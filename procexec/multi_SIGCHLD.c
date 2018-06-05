#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "../time/curr_time.h"
#include "./print_wait_status.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

extern int errno;
static volatile int numLiveChildren = 0;   

static void sigchldHandler(int sig){
    
    int savedErrno = errno; //暂存全局变量errno的初始值

    printf("%s handler: Caught SIGCHLD\n", currTime("%T"));

    int status;
    pid_t childPid;

    while((childPid = waitpid(-1, &status, WNOHANG)) > 0){
        printf("%s handler: Reaped child %ld - ", currTime("%T"), (long)childPid);
        printWaitStatus(NULL, status);
        numLiveChildren--;
    }

    if(childPid == -1 && errno != ECHILD)   errMsg("waitpid");
    sleep(5);   //拖延处理时间
    printf("%s handler: returning\n", currTime("%T"));
    errno = savedErrno; //最后还原回去
}

int main(int argc, char *argv[]){

    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s child-sleep-time...\n", argv[0]);
    }

    setbuf(stdout, NULL); 
    sigset_t blockMask, emptyMask;
    struct sigaction sa;
    numLiveChildren = argc - 1;

    //给SIGCHLD信号注册处理器
    sigemptyset(&sa.sa_mask); 
    sa.sa_flags = 0;
    sa.sa_handler = sigchldHandler;
    if(sigaction(SIGCHLD, &sa, NULL) == -1) errExit("sigaction");

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if(sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)    errExit("sigprocmask");

    //开始创建子进程
    for(int j = 1; j < argc; j++){
        switch(fork()){
            case -1 :
                errExit("fork");
            case 0 :
                sleep(getInt(argv[j], GN_NONNEG, "child-sleep-time"));
                printf("%s Child %d (PID=%ld) exiting\n", currTime("%T"), 
                        j, (long)getpid());
                _exit(EXIT_SUCCESS);
            default :
                break;
        }
    }

    int sigCnt = 0; //
    sigemptyset(&emptyMask);
    //主进程挂起，等候所有子进程运行完毕
    while(numLiveChildren > 0){
        if(sigsuspend(&emptyMask) == -1 && errno != EINTR)  errExit("sigsuspend");
        sigCnt++;
    }

    //所有子进程都已经完事
    printf("%s All %d children have terminated; SIGCHLD was caught %d times\n", 
            currTime("%T"), argc -1, sigCnt);
    exit(EXIT_SUCCESS);
}