#include "../lib/error_functions.h"
#include "../time/curr_time.h"
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define SYNC_SIG SIGUSR1

static void handler(int sig){
    //什么也不做
}

int main(int argc, char *argv[]){
    
    setbuf(stdout, NULL);

    sigset_t blockMask, origMask, emptyMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask, SYNC_SIG);
    //将阻塞信号加入进程屏蔽信号列表
    if(sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1) errExit("sigprocmask");
    
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    //发送信号
    if(sigaction(SYNC_SIG, &sa, NULL) == -1) errExit("sigaction");

    pid_t childPid;
    switch(childPid = fork()){
        case -1 :
            errExit("fork");
        case 0 : 
            printf("[%s %ld] Child started = doing some work\n",
                    currTime("%T"), (long)getpid());
            sleep(2);
            printf("[%s %ld] Child about to signal parent\n",
                    currTime("%T"), (long)getpid());
            if(kill(getppid(), SYNC_SIG) == -1) errExit("kill");
            _exit(EXIT_SUCCESS);
        default : 
            printf("[%s %ld] Parent about to wait for signal\n",
                    currTime("%T"), (long)getpid());
            sigemptyset(&emptyMask); 
            if(sigsuspend(&emptyMask) == -1 && errno != EINTR)  errExit("sigsuspend");

            printf("[%s %ld] Parent got signal\n", currTime("%T"), (long)getpid());

            if(sigprocmask(SIG_SETMASK, &origMask, NULL) == -1) errExit("sigprocmask");
            exit(EXIT_SUCCESS);
    }
}
