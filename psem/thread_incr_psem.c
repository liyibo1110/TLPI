#include "../lib/error_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

static int glob = 0;    //共享资源
static sem_t sem;

static void *threadFunc(void *arg){
    int loops = *((int *)arg);
    int loc;
    int s;
    for(int i = 0; i < loops; i++){
        //先减小信号量
        if(sem_wait(&sem) == -1)    errExit("sem_wait");
        //先赋值，再自增，再赋值，不是原子操作
        loc = glob;
        loc++;
        glob = loc;
        //先增加信号量
        if(sem_post(&sem) == -1)    errExit("sem_post");
    }
    return NULL;
}

int main(int argc, char *argv[]){

	pthread_t t1, t2;
    int loops = (argc > 1) ? atoi(argv[1]) : 10000000;
    
    //初始化未命名信号量，线程间共享，同时初始信号量为1
    if(sem_init(&sem, 0, 1) == -1)  errExit("sem_init");
    
    int s = pthread_create(&t1, NULL, threadFunc, &loops);
    //注意线程API中不是-1才表示出错了，只要不是0,都是错
    if(s != 0)  errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, &loops);
    if(s != 0)  errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL); 
    if(s != 0)  errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL); 
    if(s != 0)  errExitEN(s, "pthread_join");
    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);

}


