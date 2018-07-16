#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>

static void notifySetup(mqd_t *mqdp);

static void threadFunc(union sigval sv){
    
    //当有新的数据，才会启动该线程
    mqd_t *mqdp = sv.sival_ptr;
    if(mq_getattr(*mqdp, &attr) == -1)  errExit("mq_getattr");

    void *buffer = malloc(attr.mq_msgsize);
     if(buffer == NULL)  errExit("malloc");

    //还是先重新注册，再去处理数据
    notifySetup(mqdp);

      //因为打开了非阻塞开关，因此receive调用会立即返回，如果无数据，则返回EAGAIN错误
    while((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0){
        printf("Read %ld bytes\n", (long)numRead);
    }
    if(errno != EAGAIN) errExit("mq_receive");
}

/**
 * 封装了注册线程通知的过程
 */ 
static void notifySetup(mqd_t *mqdp){

    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = threadFunc;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = mqdp;   //传给线程函数的参数
    //首次注册消息通知，注意会立即返回
    if(mq_notify(*mqdp, &sev) == -1)  errExit("mq_notify");
}

int main(int argc, char *argv[]){
    
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s mq-name\n", argv[0]);
    }

    mqd_t mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if(mqd == (mqd_t)-1)    errExit("mq_open");

    //主线程首次注册，之后阻塞
    notifySetup(&mqd);
    pause();
}