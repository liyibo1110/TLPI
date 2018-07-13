#include "../lib/error_functions.h"
#include "./itimerspec_from_str.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>
#include <sys/timerfd.h>
#include <stdint.h>

int main(int argc, char *argv[]){
    
    if(argc < 2){
        usageErr("%s secs[/nsecs][:int-secs[/int-nsecs]] [max-exp]\n", argv[0]);
    }

    struct itimerspec ts;
    itimerspecFromStr(argv[1], &ts);

    int maxExp = (argc > 2) ? atoi(argv[2]) : 1;
    int fd = timerfd_create(CLOCK_REALTIME, 0);
    if(fd == -1)    errExit("timerfd_create");

    if(timer_settime(fd, 0, &ts, NULL) == -1)   errExit("timer_settime");

    struct timespec start, now;
    if(clock_gettime(CLOCK_MONOTONIC, &start) == -1)    errExit("clock_gettime");

    int totalExp = 0;
    int secs, nanosecs;
    uint64_t numExp, totalExp;
    ssize_t s;
    while(totalExp < maxExp){
        s = read(fd, &numExp, sizeof(uint64_t));
        if(s != sizeof(uint64_t))   errExit("read");
        totalExp += numExp;

        if(clock_gettime(CLOCK_MONOTONIC, &now) == -1)  errExit("clock_gettime");
        secs = now.tv_sec - start.tv_sec;
        nanosecs = now.tv_nsec - start.tv_nsec;
        if(nanosecs < 0){   //总之不能让纳秒字段为负
            secs--;
            nanosecs += 1000000000;
        }
        printf("%d.%03d: expirations read: %llu; total=%llu\n", 
                secs, (nanosecs + 500000) / 1000000,
                (unsigned long long)numExp, (unsigned long long)totalExp);
    }

    exit(EXIT_SUCCESS);
}