#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>

int main(int argc, char *argv[]){
    if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [msqid...]\n", argv[0]);
    }

    for(int i = 1; i < argc; i++){
        if(msgctl(getInt(argv[i], 0, "msqid"), IPC_RMID, NULL) == -1){
            errExit("msgctl %s", argv[i]);
        }
    }

    exit(EXIT_SUCCESS);
}
