#define _POSIX_C_SOURCE 199309
#include "../lib/error_functions.h"
#include "../time/curr_time.h"
#include "./itimerspec_from_str.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

#define TIMER_SIG SIGRTMAX  //自定义实时信号

static void handler(int sig, siginfo_t *si, void *uc){
    
    timer_t *tidptr = si->si_value.sival_ptr;

    printf("[%s] Got signal %d\n", currTime("%T"), sig);
    printf("    *sival_ptr         = %ld\n", (long)*tidptr);
    printf("    timer_getoverrun() = %d\n", timer_getoverrun(*tidptr));
}

int main(int argc, char *argv[]){
    
    if(argc < 2){
        usageErr("%s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);
    }

    timer_t *tidlist = calloc(argc - 1, sizeof(timer_t));
    if(tidlist == NULL) errExit("calloc");

    //弄信号处理器
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if(sigaction(TIMER_SIG, &sa, NULL) == -1)   errExit("sigaction");

    //创建定时器
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = TIMER_SIG;

    struct itimerspec ts;
    for(int i = 0; i < argc - 1; i++){
        itimerspecFromStr(argv[i + 1], &ts);
        sev.sigev_value.sival_ptr = &tidlist[i];    //当作数组
        //开始计时
        if(timer_create(CLOCK_REALTIME, &sev, &tidlist[i]) == -1)   errExit("timer_create");
        printf("Timer ID: %ld (%s)\n", (long)tidlist[i], argv[i + 1]);

        if(timer_settime(tidlist[i], 0, &ts, NULL) == -1)   errExit("timer_settime");
    }

    while(true){
        pause();
    }
}