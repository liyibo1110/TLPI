#include "../lib/error_functions.h"
#include <sys/signalfd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]){

	if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s sig-num...\n", argv[0]);
    }

    printf("%s: PID is %ld\n", argv[0], (long)getpid());

    sigset_t mask;
    sigemptyset(&mask);
    for(int i = 1; i< argc; i++){
        sigaddset(&mask, atoi(argv[i]));
    }
    if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1)   errExit("sigprocmask");

    int sfd = signalfd(-1, &mask, 0);
    if(sfd == -1)   errExit("signalfd");

    ssize_t s;
    struct signalfd_siginfo fdsi;
    while(true){
        s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
        if(s != sizeof(struct signalfd_siginfo))    errExit("read");

        printf("%s: got signal %d", argv[0], fdsi.ssi_signo);
        if(fdsi.ssi_code == SI_QUEUE){
            printf("; ssi_pid = %d; ", fdsi.ssi_pid);
            printf("ssi_int = %d", fdsi.ssi_int);
        }
        printf("\n");
    }

    exit(EXIT_SUCCESS);

}


