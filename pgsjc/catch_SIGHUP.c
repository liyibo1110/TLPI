#include "../lib/error_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

static void handler(int sig){

}

int main(int argc, char *argv[]){

    setbuf(stdout, NULL);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if(sigaction(SIGHUP, &sa, NULL) == -1)  errExit("sigaction");

    pid_t childPid = fork();

    if(childPid == -1)  errExit("fork");
    if(childPid == 0 && argc > 1){
        if(setpgid(0, 0) == -1) errExit("setpgid");
    }

    printf("PID=%ld; PPID=%ld; PGID=%ld; SID=%ld\n", (long)getpid(),
            (long)getppid(), (long)getpgrp(), (long)getsid(0));

    alarm(60);
    while(true){
        pause();
        printf("%ld: caught SIGHUP\n", (long)getpid());
    }
}