#include "../lib/error_functions.h"
#include "../lib/signal_functions.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

static volatile sig_atomic_t gotSigquit = 0;

static void handler(int sig){
    printf("Caught signal %d (%s)\n", sig, strsignal(sig));
    if(sig == SIGQUIT){
        gotSigquit = 1;
    }
}

int main(int argc, char *argv[]){

	printSigMask(stdout, "Initial signal mask is:\n");
	
    //屏蔽INT和QUIT信号
    sigset_t blockMask, origMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGINT);
    sigaddset(&blockMask, SIGQUIT);
    if(sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1) errExit("sigprocmask - SIG_BLOCK");
    
    //信号处理器绑定INT和QUIT信号
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if(sigaction(SIGINT, &sa, NULL) == -1)  errExit("sigaction");
    if(sigaction(SIGQUIT, &sa, NULL) == -1)  errExit("sigaction");
    
    for(int i = 1; !gotSigquit; i++){
        printf("=== LOOP %d\n", i);
        printSigMask(stdout, "Starting critical section, signal mask is:\n");
        //空转一会儿，要空转而不是让它休眠
        for(time_t startTime = time(NULL); time(NULL) < startTime + 4; ){
            continue;
        }
        printPendingSigs(stdout, "Before sigsuspend() - pending signals:\n");
        //临时解除INT和QUIT信号的屏蔽，同时挂起，直接获取新的信号并且成功在处理器中处理
        if(sigsuspend(&origMask) == -1 && errno != EINTR)   errExit("sigsuspend");
        //直接到遇到了QUIT信号（Ctrl - \），才会退出，不然无限循环
    }

    //取消信号屏蔽
    if(sigprocmask(SIG_SETMASK, &origMask, NULL) == -1) errExit("sigprocmask - SIG_SETMARK");
    printSigMask(stdout, "=== Exited loop\nRestored signal mask to:\n");
    exit(EXIT_SUCCESS);

}


