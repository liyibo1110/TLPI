#include "../lib/error_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;

static int totalThreads = 0;    //创建的线程数
static int numLive = 0; //活着的，或者中止但没有被join的线程数
static int numUnjoined = 0; //已中止但没有被join的线程数

//定义线程的3种状态
enum tstate{
    TS_ALIVE,
    TS_TERMINATED,
    TS_JOINED
};

//定义线程的抽象结构
static struct{
    pthread_t tid;
    enum tstate state;
    int sleepTime;  //存活时间
} *thread;

static void *threadFunc(void *arg){
    
    //参数传过来的是创建线程的数组下标
    int index = (int) arg;
    int s;
    
    sleep(thread[index].sleepTime);
    printf("Thread %d terminating\n", index);
    
    s = pthread_mutex_lock(&threadMutex);
    if(s != 0)  errExitEN(s, "pthread_mutex_lock");

    //开始操作共享资源
    numUnjoined++;
    thread[index].state = TS_TERMINATED;

    s = pthread_mutex_unlock(&threadMutex);
    if(s != 0)  errExitEN(s, "pthread_mutex_unlock");
  
    s = pthread_cond_signal(&threadDied);
    if(s != 0)  errExitEN(s, "pthread_cond_signal");

    return NULL;
}

int main(int argc, char *argv[]){

    int s;

	if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s num-secs...\n", argv[0]);
    }
    
    thread = calloc(argc - 1, sizeof(*thread));
    if(thread == NULL)  errExit("calloc");

    //开始创建线程
    for(int index = 0; index < argc - 1; index++){
        thread[index].sleepTime = atoi(argv[index + 1]);
        thread[index].state = TS_ALIVE;
        s = pthread_create(&thread[index].tid, NULL, threadFunc, (void *) index);   //index必须要传地址
        if(s != 0)  errExitEN(s, "pthread_create");
    }

    totalThreads = argc - 1;
    numLive = totalThreads;

    //开始等待子线程
    while(numLive > 0){
        s = pthread_mutex_lock(&threadMutex);
        if(s != 0)  errExitEN(s, "pthread_mutex_lock");

        //开始操作共享资源
        while(numUnjoined == 0){    //如果还没有线程终止，就等待
            s = pthread_cond_wait(&threadDied, &threadMutex);
            if(s != 0)  errExitEN(s, "pthread_cond_wait");
        }

        //每次轮询所有线程
        for(int index = 0; index < totalThreads; index++){
            if(thread[index].state == TS_TERMINATED){
                s = pthread_join(thread[index].tid, NULL);
                if(s != 0)  errExitEN(s, "pthread_join");
                thread[index].state = TS_JOINED;
                numLive--;
                numUnjoined--;
                printf("Reaped thread %d (numLive=%d)\n", index, numLive);
            }
        }

        s = pthread_mutex_unlock(&threadMutex);
        if(s != 0)  errExitEN(s, "pthread_mutex_unlock");
    }
    
    exit(EXIT_SUCCESS);
}


