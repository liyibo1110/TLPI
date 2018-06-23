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
    if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [semid...]\n", argv[0]);
    }

    union semun dummy;

    for(int i = 1; i < argc; i++){
        if(semctl(getInt(argv[i], 0, "semid"), 0, IPC_RMID, &dummy) == -1){
            errExit("semctl %s", argv[i]);
        }
    }

    exit(EXIT_SUCCESS);
}
