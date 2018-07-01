#include "../lib/error_functions.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]){

	if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [delay-secs]\n", argv[0]);
    }

    printf("%s: PID is %ld\n", argv[0], (long)getpid());

    //屏蔽所有信号(默认不包括KILL和STOP信号)
    sigset_t allSigs;
    sigfillset(&allSigs);
    if(sigprocmask(SIG_SETMASK, &allSigs, NULL) == -1)  errExit("sigprocmask");
    printf("%s: signals blocks\n", argv[0]);

    //开始休眠
    if(argc > 1){
        printf("%s: about to delay %s seconds\n", argv[0], argv[1]);
        sleep(atoi(argv[1]));
        printf("%s: finished delay\n", argv[0]);
    }

    siginfo_t si;
    int sig;
    while(true){
        sig = sigwaitinfo(&allSigs, &si);
        if(sig == -1)   errExit("sigwaitinfo");
        if(sig == SIGINT || sig == SIGTERM) exit(EXIT_SUCCESS);
        printf("got signal: %d (%s)\n", sig, strsignal(sig));
        printf("    si_signo=%d, si_code=%d (%s), si_value=%d\n",
                si.si_signo, si.si_code, 
                (si.si_code == SI_USER) ? "SI_USER" : (si.si_code == SI_QUEUE) ? "SI_QUEUE" : "other",
                si.si_value.sival_int);
        printf("    si_pid=%ld, si_uid=%ld\n", (long)si.si_pid, (long)si.si_uid);
    }

    exit(EXIT_SUCCESS);

}


