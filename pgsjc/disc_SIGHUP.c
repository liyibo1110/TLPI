#include "../lib/error_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

static void handler(int sig){
    printf("PID %ld: caught signal %2d (%s)\n", (long)getpid(),
            sig, strsignal(sig));
}

int main(int argc, char *argv[]){

    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s {d|s}... [ > sig.log 2>&1 ]\n", argv[0]);
    }

    setbuf(stdout, NULL);

    pid_t parentPid = getpid();
    printf("PID of parent process is:   %ld\n", (long)parentPid);
    printf("Foreground process group ID is: %ld\n", (long)tcgetpgrp(STDIN_FILENO));

    pid_t childPid;
    struct sigaction sa;
    for(int i = 1; i < argc; i++){
        childPid = fork();
        if(childPid == -1)  errExit("fork");
        if(childPid == 0){
            if(argv[i][0] == 'd'){  //如果传来d，则移到一个新的进程组
                if(setpgid(0, 0) == -1) errExit("setpgid");
            }
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = handler;
            if(sigaction(SIGHUP, &sa, NULL) == -1)  errExit("sigaction");
            break;
        }
    }

    alarm(60);  //确保所有进程最终都会终止
    printf("PID=%ld PGID=%ld\n", (long)getpid(), (long)getpgrp());
    while(true){
        pause();
    }
}