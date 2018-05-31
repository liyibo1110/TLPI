#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "../time/curr_time.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/wait.h>
int main(int argc, char *argv[]){
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s sleep-time...\n", argv[0]);
    }

    setbuf(stdout, NULL);

    for(int j = 1; j < argc; j++){
        switch(fork()){
            case -1 :
                errExit("fork");
            case 0 :
                printf("[%s] child %d started with PID %ld, sleeping %s seconds\n",
                        currTime("%T"), j, (long)getpid(), argv[j]);
                sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
                _exit(EXIT_SUCCESS);    //一定要退出
            default : 
                break;
        }
    }

    //以下为主进程的剩余部分
    int numDead = 0;
    pid_t childPid;
    while(true){
         childPid = wait(NULL);
         if(childPid == -1){
            //检查是否为正常结束子进程
            if(errno == ECHILD){    //pid为-1同时标记了特定的errno，可认为所有子进程执行完毕，否则为程序错误
                printf("No more children - bye!\n");
                exit(EXIT_SUCCESS);
            }else{
                errExit("wait");
            }
         }
         numDead++;
         printf("[%s] wait() returned child PID %ld (numDead=%d)\n",
                 currTime("%T"), (long)childPid, numDead);

    }
} 
