#include "../lib/error_functions.h"
#include "./semun.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>

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
int reserveSem(int semId, int semNum);
int releaseSem(int semId, int semNum);