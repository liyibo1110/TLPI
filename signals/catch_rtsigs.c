#include "../lib/error_functions.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

static volatile int handlerSleepTime;
static volatile int sigCount = 0;   //收到的信号数
static volatile bool allDone = false;

static void siginfoHandler(int sig, siginfo_t *si, void *ucontext){
    //如果是终止信号，直接终止
    if(sig == SIGINT || sig == SIGTERM){
        allDone = true;
        return;
    }
    sigCount++;
    printf("Caught signal %d\n", sig);
    printf("    si_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
            (si->si_code == SI_USER) ? "SI_USER" : 
            (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
    printf("si_value=%d\n", si->si_value.sival_int);
    printf("    si_pid=%ld, si_uid=%ld\n", (long)si->si_pid, (long)si->si_uid);
    //休息一会儿，为了观察信号排队情况
    sleep(handlerSleepTime);
}

int main(int argc, char *argv[]){

	if(argc > 1 && strcmp(argv[1], "--help") == 0){
		usageErr("%s [block-time [handler-sleep-time]]\n", argv[0]);
	}
    
    printf("%s: PID is %ld\n", argv[0], (long)getpid());

    handlerSleepTime = (argc > 2) ? atoi(argv[2]) : 1;

    struct sigaction sa;
    sa.sa_sigaction = siginfoHandler;
    sa.sa_flags = SA_SIGINFO;
    sigfillset(&sa.sa_mask);    //在信号处理器内部执行中，屏蔽所有其他信号

    //捕获几乎所有的信号
    for(int sig = 1; sig < NSIG; sig++){
        if(sig == SIGTSTP || sig == SIGQUIT){
            continue;
        }
        sigaction(sig, &sa, NULL);
    }
    
    if(argc > 1){
        sigset_t blockMask, prevMask;
        sigfillset(&blockMask);
        sigdelset(&blockMask, SIGINT);
        sigdelset(&blockMask, SIGTERM);
    
        if(sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1){
            errExit("sigprocmask");
        }

        printf("%s: signals blocked - sleeping %s seconds\n", argv[0], argv[1]);
        sleep(atoi(argv[1]));
        //休眠之后取消所有信号阻塞的设置
        if(sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1) errExit("sigprocmask");
    }

    while(!allDone){
        pause();    //处理完信号才会恢复
    }
	exit(EXIT_SUCCESS);

}


