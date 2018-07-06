#include "../lib/error_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int glob = 0;

static void cleanupHandler(void *arg){
    printf("cleanup: freeing block at %p\n", arg);
    free(arg);
    printf("cleanup: unlocking mutex\n");
    int s = pthread_mutex_unlock(&mtx);
    if(s != 0)  errExitEN(s, "pthread_mutex_unlock");
}

static void *threadFunc(void *arg){
    
    void *buf = malloc(0x10000);
    printf("thread: allocated memory at %p\n", buf);

    int s = pthread_mutex_lock(&mtx);
    if(s != 0)  errExitEN(s, "pthread_mutex_lock");
    pthread_cleanup_push(cleanupHandler, buf);

    //等待全局变量glob不为0,才结束
    while(glob == 0){
        s = pthread_cond_wait(&cond, &mtx);
        if(s != 0)  errExitEN(s, "pthread_cond_wait");
    }
    pthread_cleanup_pop(1); //不管线程有没有被取消，都会执行cleanup函数进行清理
    return NULL;
}

int main(int argc, char *argv[]){

    pthread_t tid;
    int s = pthread_create(&tid, NULL, threadFunc, NULL);
    if(s != 0)  errExitEN(s, "pthread_create");

    sleep(2);   //让子线程运行到wait阻塞

    if(argc == 1){  //取消子线程
        printf("main:   about to cancel thread\n");
        s = pthread_cancel(tid);
        if(s != 0)  errExitEN(s, "pthread_cancel");
    }else{  //让子线程继续执行完毕
        printf("main:   about to signal condition variable\n");
        glob = 1;
        s = pthread_cond_signal(&cond);
        if(s != 0)  errExitEN(s, "pthread_cond_signal");
    }

    void *res;
    s = pthread_join(tid, &res);
    if(s != 0)  errExitEN(s, "pthread_join");
    if(res == PTHREAD_CANCELED){
        printf("main:   thread was canceled\n");
    }else{
        printf("main:   thread terminated normally\n");
    }

    exit(EXIT_SUCCESS);
}


