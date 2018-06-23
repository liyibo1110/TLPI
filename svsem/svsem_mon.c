#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "./semun.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>

int main(int argc, char *argv[]){
    
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s semid\n", argv[0]);
    }

    int semid = getInt(argv[1], 0, "semid");
    struct semid_ds ds;
    union semun arg, dummy;

    arg.buf = &ds;  //挂上
    //ds里就有东西了
    if(semctl(semid, 0, IPC_STAT, arg) == -1)   errExit("semctl");  

    printf("Semaphore changed: %s", ctime(&ds.sem_ctime));
    printf("Last semop():      %s", ctime(&ds.sem_otime));

    //显示每个信号量
    arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));   //array需要动态分配，因为不知道ds里面会有多少个信号
    if(arg.array == NULL)   errExit("calloc");
    if(semctl(semid, 0, GETALL, arg) == -1) errExit("semctl - GETALL");

    printf("Sem #   Value   SEMPID  SEMNCNT SEMZCNT\n");
    for(int i = 0; i < ds.sem_nsems; i++){
        printf("%3d %5d %5d %5d %5d\n", i, arg.array[i],
                semctl(semid, i, GETPID, dummy),
                semctl(semid, i, GETNCNT, dummy),
                semctl(semid, i, GETZCNT, dummy));
    }
    exit(EXIT_SUCCESS);
}