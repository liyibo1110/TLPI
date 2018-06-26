#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>

int main(int argc, char *argv[]){
    
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s mq-name\n", argv[0]);
    }

    if(mq_unlink(argv[1]) == -1)    errExit("mq_unlink");
    exit(EXIT_SUCCESS);
}
