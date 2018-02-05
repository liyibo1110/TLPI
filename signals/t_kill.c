#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../lib/error_functions.h"
#include "../lib/get_num.h"
int main(int argc, char *argv[]){
    if (argc != 3 || strcmp(argv[1], "--help") == 0){
        usageErr("%s sig-num pid\n", argv[0]);
    }

    int sig = getInt(argv[1], 0, "sig-num");
    int pid = getLong(argv[2], 0, "pid");
    int s = kill(pid, sig);
    
    if(sig != 0){
        if(s == -1){
            errExit("kill");
        }
    }else{
        if(s == 0){ //发送了sig为0的空信号
            printf("Process exists and we can send it a signal\n");
        }else{
            if(errno == EPERM){
                printf("Process exists, but we don't have permission to send it a signal\n");
            }else if(errno == ESRCH){
                printf("Process does not exist\n");
            }else{
                errExit("kill");
            }
        }
    }
    exit(EXIT_SUCCESS);
}