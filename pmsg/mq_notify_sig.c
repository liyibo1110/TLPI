#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>

#define NOTIFY_SIG SIGUSR1  //借用标准信号的备胎

static void handler(int sig){
    
    //什么也不做
}

int main(int argc, char *argv[]){
    
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s mq-name\n", argv[0]);
    }

    mqd_t mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if(mqd == (mqd_t)-1)    errExit("mq_open");

    struct mq_attr attr;
    if(mq_getattr(mqd, &attr) == -1)    errExit("mq_getattr");

    void *buffer = malloc(attr.mq_msgsize);
    if(buffer == NULL)  errExit("malloc");

    sigset_t blockMask, emptyMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask, NOTIFY_SIG);
    //先阻塞通知信号
    if(sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1)  errExit("sigprocmask");

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if(sigaction(NOTIFY_SIG, &sa, NULL) == -1)  errExit("sigaction");

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    //首次注册消息通知，注意会立即返回
    if(mq_notify(mqd, &sev) == -1)  errExit("mq_notify");

    sigemptyset(&emptyMask);
    ssize_t numRead;
    while(true){
        //在这里挂起并等待通知信号
        sigsuspend(&emptyMask);
        //到这里说明之前收到了通知，需要再次立即先注册，然后紧跟数据处理
        if(mq_notify(mqd, &sev) == -1)  errExit("mq_notify");
        //因为打开了非阻塞开关，因此receive调用会立即返回，如果无数据，则返回EAGAIN错误
        while((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0){
            printf("Read %ld bytes\n", (long)numRead);
        }
        if(errno != EAGAIN) errExit("mq_receive");
    }
}