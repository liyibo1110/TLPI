#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define BUF_SIZE 200

extern int errno;

static void handler(int sig){
    printf("Caught signal\n");
}

int main(int argc, char *argv[]){
    struct sigaction sa;
    
    if(argc > 1 && strcmp(argv[1], "help") == 0){  
        usageErr("%s [num-secs [restart-flag]]\n", argv[0]);
    }

    //注册信号处理器
    sa.sa_flags = (argc > 2) ? SA_RESTART : 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    if(sigaction(SIGALRM, &sa, NULL) == -1) errExit("sigaction");

    alarm((argc > 1) ? atoi(argv[1]) : 10);

    char buf[BUF_SIZE];
    //从键盘读取（在alarm到期之前）
    ssize_t numRead = read(STDIN_FILENO, buf, BUF_SIZE - 1);   

    //确保从read系统调用到检查期间，任何行为不会改变errno
    int savedErrno = errno;
    alarm(0);   //如果没有超时，则必须人工关闭定时器，这样就不会收到信号了
    errno = savedErrno;

    //检查结果
    if(numRead == -1){
        if(errno == EINTR){ //到这里说明是超时
            printf("Read timed out\n");
        }else{
            errMsg("read");
        }
    }else{
        printf("Successful read (%ld bytes): %.*s", (long)numRead, (int)numRead, buf);
    }
    exit(EXIT_SUCCESS);
}