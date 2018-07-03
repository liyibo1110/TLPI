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
    //这样极大浪费CPU资源，因为大部分时间都是无意义的检查
    while(true){
        //主线程也得抢锁
        s = pthread_mutex_lock(&mtx);
        if(s != 0)  errExitEN(s, "pthread_mutex_lock");
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


