#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "../time/curr_time.h"
#include "./semun.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>

int main(int argc, char *argv[]){
    
    //要么1个参数，要么2个参数，业务不一样
    if(argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0){
        usageErr("%s init-value\n   or: %s semid operation\n", argv[0], argv[0]);
    }

    int  semid;
    if(argc == 2){
        //如果1个参数，则创建新的只有1个信号量的组，参数为信号量的值
        union semun arg;
        semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
        if(semid == -1) errExit("semid");
        arg.val = getInt(argv[1], 0, "init-value"); //val字段为信号量的值
        if(semctl(semid, 0, SETVAL, arg) == -1) errExit("semctl");
        printf("Semaphore ID = %d\n", semid);
    }else{
        //如果2个参数，则参数1对应的信号量标识符，增加参数2的值
        struct sembuf sop;
        semid = getInt(argv[1], 0, "semid");

        sop.sem_num = 0;    //指定要操作哪个序号的信号
        sop.sem_op = getInt(argv[2], 0, "operation");
        sop.sem_flg = 0;    //无特别标志

        printf("%ld: about to semop at %s\n", (long)getpid(), currTime("%T"));

        //开始修改
        if(semop(semid, &sop, 1) == -1) errExit("semop");

        printf("%ld: semop completed at %s\n", (long)getpid(), currTime("%T"));
    }

    exit(EXIT_SUCCESS);
}