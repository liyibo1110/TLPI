#include "../lib/error_functions.h"
#include "./semun.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>

extern int errno;

bool bsUseSemUndo = false;
bool bsRetryOnEintr = true;

int initSemAvailable(int semId, int semNum){
    union semun arg;
    arg.val = 1;
    return semctl(semId, semNum, SETVAL, arg);
}

int initSemInUse(int semId, int semNum){
    union semun arg;
    arg.val = 0;
    return semctl(semId, semNum, SETVAL, arg);
}
int reserveSem(int semId, int semNum){
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
    while(semop(semId, &sops, 1) == -1){    //考虑会阻塞的问题
        if(errno != EINTR || !bsRetryOnEintr) return -1;
    }
    return 0;
}
int releaseSem(int semId, int semNum){
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
    return semop(semId, &sops, 1);  //释放不考虑阻塞的问题
}