#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

extern int errno;

static void handler(int sig){
    return; //什么也不做
}

int main(int argc, char *argv[]){
    
    if(argc != 3 && strcmp(argv[1], "help") == 0){  
        usageErr("%s secs nanosecs\n", argv[0]);
    }

    struct timespec request;
    struct timespec remain;
    request.tv_sec = strtol(argv[1], NULL, 10);
    request.tv_nsec = strtol(argv[2], NULL, 10);

    //注册信号处理器
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if(sigaction(SIGINT, &sa, NULL) == -1) errExit("sigaction");

    struct timeval start;
    struct timeval finish;

    if(gettimeofday(&start, NULL) == -1)    errExit("gettimeofday");
    
    int s;
    while(true){
        s = nanosleep(&request, &remain);
        if(s == -1 && errno != EINTR)   errExit("nanosleep");
        if(gettimeofday(&finish, NULL) == -1)    errExit("gettimeofday");
        printf("Slept for: %9.6f secs\n", finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0);
        if(s == 0)  break;  //说明到时间了
        printf("Remaining: %2ld.%09ld\n", (long)remain.tv_sec, remain.tv_nsec);
        request = remain;   //恢复休眠
    }

    printf("Sleep complete\n");
    exit(EXIT_SUCCESS);
}