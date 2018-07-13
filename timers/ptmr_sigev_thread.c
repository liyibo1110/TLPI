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
#include <pthread.h>
#include <stdbool.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int expireCount;

static void threadFunc(union sigval sv){
    
    timer_t *tidptr = sv.sival_ptr;

    printf("[%s] Thread notify\n", currTime("%T"));
    printf("    timer ID= %ld\n", (long)*tidptr);
    printf("    timer_getoverrun() = %d\n", timer_getoverrun(*tidptr));

    int s = pthread_mutex_lock(&mtx);
    if(s != 0)  errExitEN(s, "pthread_mutex_lock");
    expireCount += 1 + timer_getoverrun(*tidptr);   //还要尝试追加溢出的到时通知
    s = pthread_mutex_unlock(&mtx);
    if(s != 0)  errExitEN(s, "pthread_mutex_unlock");

    s = pthread_cond_signal(&cond);
    if(s != 0)  errExitEN(s, "pthread_cond_signal");
}

int main(int argc, char *argv[]){
    
    if(argc < 2){
        usageErr("%s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);
    }

    timer_t *tidlist = calloc(argc - 1, sizeof(timer_t));
    if(tidlist == NULL) errExit("calloc");

    //弄线程通知器
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = threadFunc;
    sev.sigev_notify_attributes = NULL;

    struct itimerspec ts;
    for(int i = 0; i < argc - 1; i++){
        itimerspecFromStr(argv[i + 1], &ts);
        sev.sigev_value.sival_ptr = &tidlist[i];    //当作数组
        //开始计时
        if(timer_create(CLOCK_REALTIME, &sev, &tidlist[i]) == -1)   errExit("timer_create");
        printf("Timer ID: %ld (%s)\n", (long)tidlist[i], argv[i + 1]);

        if(timer_settime(tidlist[i], 0, &ts, NULL) == -1)   errExit("timer_settime");
    }

    int s = pthread_mutex_lock(&mtx);   //立刻先锁定，不让线程在调用wait之前操作
    if(s != 0)  errExitEN(s, "pthread_mutex_lock");
    
    while(true){
        s = pthread_cond_wait(&cond, &mtx); //这一步会自动unlock
        if(s != 0)  errExitEN(s, "pthread_cond_wait");
        printf("main(): expireCount = %d\n", expireCount);
    }
}