#include "../lib/error_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

static int avail = 0;    //子线程产生的资源
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void *threadFunc(void *arg){
    int count = atoi((char *)arg);
    int s;
    for(int i = 0; i < count; i++){
        sleep(1);   //耗1秒，假装在工作
        //先锁定
        s = pthread_mutex_lock(&mtx);
        if(s != 0)  errExitEN(s, "pthread_mutex_lock");
        
        avail++;

        s = pthread_mutex_unlock(&mtx);
        if(s != 0)  errExitEN(s, "pthread_mutex_unlock");

        //使用cond来通知主线程，也可以先发送，再解锁，属于高端问题了
        s = pthread_cond_signal(&cond);
        if(s != 0)  errExitEN(s, "pthread_cond_signal");
    }
    return NULL;
}

int main(int argc, char *argv[]){

	pthread_t tid;
    int totalRequired = 0;
    int numConsumed = 0;
    time_t start = time(NULL);
    int s;
    for(int i = 1; i < argc; i++){
        totalRequired += atoi(argv[i]);
        s = pthread_create(&tid, NULL, threadFunc, argv[i]);
        if(s != 0)  errExitEN(s, "pthread_create");
    }

    //继续主线程
    bool done = false;

    while(true){
        //主线程也得抢锁
        s = pthread_mutex_lock(&mtx);
        if(s != 0)  errExitEN(s, "pthread_mutex_lock");
        
        //当没有资源时，老实阻塞等待子线程通知
        while(avail == 0){
            s = pthread_cond_wait(&cond, &mtx);
            if(s != 0)  errExitEN(s, "pthread_cond_wait");
        }
        
        //因为在锁定中，所以到了这一步也不怕，但是必须要走判断，不能wait完事了就直接当作有资源了
        while(avail > 0){
            numConsumed++;
            avail--;
            printf("T=%ld: numConsumed=%d\n", (long)(time(NULL) - start), numConsumed);
            //都消费了才改变状态
            done = numConsumed >= totalRequired;
        }
        s = pthread_mutex_unlock(&mtx);
        if(s != 0)  errExitEN(s, "pthread_mutex_unlock");
        if(done){
            break;
        }
    }
    
    exit(EXIT_SUCCESS);
}


