#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "./print_wait_status.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    
    if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [exit-status]\n", argv[0]);
    }

    pid_t childPid;
    int status;
    switch(fork()){
        case -1 :
            errExit("fork");
        case 0 : 
            printf("Child started with PID = %ld\n", (long)getpid());
            if(argc > 1){
                exit(getInt(argv[1], 0, "exit-status"));
            }else{  //没有参数，则直接挂起，等待接收信号
                while(true) pause();
            }
            exit(EXIT_FAILURE);
        default :
            while(true){
                childPid = waitpid(-1, &status, WUNTRACED
                #ifdef WCONTINUED
                    |   WCONTINUED
                #endif        
                );
                if(childPid == -1)  errExit("waitpid");
                //开始打印子进程的结束状态
                printf("waitpid() returned: PID=%ld; status=0x%04x (%d,%d)\n",
                        (long)childPid, (unsigned int)status, status >> 8, status & 0xff);
                printWaitStatus(NULL, status);
                if(WIFEXITED(status) || WIFSIGNALED(status))    exit(EXIT_SUCCESS);
            }
    }    
}
