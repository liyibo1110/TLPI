#include "../lib/error_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

static void *threadFunc(void *arg){
    
    int i = 1;
    printf("New thread started\n");
    while(true){
        printf("Loop %d\n", i);
        sleep(1);
        i++;
    }
    return NULL;
}

int main(int argc, char *argv[]){

	pthread_t tid;
    int s = pthread_create(&tid, NULL, threadFunc, NULL);
    //注意线程API中不是-1才表示出错了，只要不是0,都是错
    if(s != 0)  errExitEN(s, "pthread_create");

    sleep(3);   //让子线程运行几次

    s = pthread_cancel(tid);
    if(s != 0)  errExitEN(s, "pthread_cancel");

    void *res;
    s = pthread_join(tid, &res); //注意res本身是指针，但还是要取地址，因为要改变自身
    if(s != 0)  errExitEN(s, "pthread_join");
    
    if(res == PTHREAD_CANCELED){
        printf("Thread was canceled\n");
    }else{
        printf("Thread wsa not canceled (should not happen!)\n");
    }

    exit(EXIT_SUCCESS);
}


