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

    if(sem_unlink(argv[1]) == -1)    errExit("sem_unlink");
    exit(EXIT_SUCCESS);
}
