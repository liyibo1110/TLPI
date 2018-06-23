#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "./semun.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>

int main(int argc, char *argv[]){
    
    //至少要有2个参数
    if(argc < 3 || strcmp(argv[1], "--help") == 0){
        usageErr("%s semid val...\n", argv[0]);
    }

    int semid = getInt(argv[1], 0, "semid");
    struct semid_ds ds;
    union semun arg;
    arg.buf = &ds;

    if(semctl(semid, 0, IPC_STAT, arg) == -1)   errExit("semctl");

    if(ds.sem_nsems != argc - 2){
        cmdLineErr("args number error\n");
    }

    arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));
    if(arg.array == NULL)   errExit("calloc");

    //将命令行最后的要修改值直接给array
    for(int i = 2; i < argc; i++){
        arg.array[i - 2] = getInt(argv[i], 0, "val");
    }

    if(semctl(semid, 0, SETALL, arg) == -1) errExit("semctl - SETALL");

    printf("Semaphore values changed (PID=%ld)\n", (long)getpid());

    exit(EXIT_SUCCESS);
}