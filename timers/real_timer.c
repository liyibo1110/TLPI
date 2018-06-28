#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

static volatile sig_atomic_t gotAlarm = 0;

static void displayTimes(const char *msg, bool includeTimer){
    static int callNum = 0; //计数器
    static struct timeval start;   
    struct timeval curr;
    struct itimerval itv;
    if(callNum == 0){
        if(gettimeofday(&start, NULL) == -1)    errExit("gettimeofday");
    }
    if(callNum % 20 == 0){  //每隔20次输出一轮
        printf("    Elapsed     Value   Interval\n");
    }

    if(gettimeofday(&curr, NULL) == -1) errExit("gettimeofday");
    //打印已经过的时间
    printf("%-7s %6.2f", msg, curr.tv_sec - start.tv_sec + (curr.tv_usec - start.tv_usec) / 1000000.0);

    //打印定时器的2个时间字段
    if(includeTimer){
        if(getitimer(ITIMER_REAL, &itv) == -1)  errExit("getitimer");
        printf("    %6.2f   %6.2f",
                itv.it_value.tv_sec + itv.it_value.tv_usec / 1000000.0,
                itv.it_interval.tv_sec + itv.it_interval.tv_usec / 1000000.0);
    }
    printf("\n");
    callNum++;
}

static void sigAlarmHandler(int sig){
    gotAlarm = 1;
}

int main(int argc, char *argv[]){
    struct itimerval itv;
    struct sigaction sa;
    
    if(argc > 1 && strcmp(argv[1], "help") == 0){  
        usageErr("%s [secs [usecs [int-secs [int-usecs]]]]\n", argv[0]);
    }

    //初始化信号处理器
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigAlarmHandler;
    if(sigaction(SIGALRM, &sa, NULL) == -1) errExit("sigaction");

    //如果定义的是一次性的定时器，则只接收1次信号，否则接收3次
    int maxSigs = (itv.it_interval.tv_sec == 0 && itv.it_interval.tv_usec == 0) ? 1 : 3;

    displayTimes("START:", false);

    //给定时器赋值
    itv.it_value.tv_sec = (argc > 1) ? strtol(argv[1], NULL, 10) : 2;
    itv.it_value.tv_usec = (argc > 2) ? strtol(argv[2], NULL, 10) : 0;
    itv.it_interval.tv_sec = (argc > 3) ? strtol(argv[3], NULL, 10) : 0;
    itv.it_interval.tv_usec = (argc > 4) ? strtol(argv[4], NULL, 10) : 0;
    if(setitimer(ITIMER_REAL, &itv, 0) == -1)   errExit("setitimer");
    clock_t prevClock = clock();
    int sigCount = 0;

    while(true){
        while(((clock() - prevClock) * 10 / CLOCKS_PER_SEC) < 5){
            if(gotAlarm){
                gotAlarm = 0;
                displayTimes("ALARM:", true);
                sigCount++;
                if(sigCount >= maxSigs){
                    printf("finished!\n");
                    exit(EXIT_SUCCESS);
                }
            }
        }
        prevClock = clock();
        displayTimes("Main: ", true);
    }
}