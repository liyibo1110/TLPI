#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>

int main(int argc, char *argv[]){
    
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s sem-name\n", argv[0]);
    }

    sem_t *sem = sem_open(argv[1], 0);
    if(sem == SEM_FAILED)   errExit("sem_open");
    if(sem_post(sem) == -1)    errExit("sem_post");

    printf("%ld sem_post() succeeded\n", (long)getpid());

    exit(EXIT_SUCCESS);
}
