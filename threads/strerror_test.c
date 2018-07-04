#include "../lib/error_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

/**
 * 线程要执行的函数
 */ 
static void *threadFunc(void *arg){

    printf("Other thread about to call strerror()\n");
    char *str = strerror(EPERM);
    printf("Other thread: str (%p) = %s\n", str, str);
    return NULL;
}

int main(int argc, char *argv[]){

    char *str = strerror(EINVAL);
    printf("Main thread has called strerror()\n");

	pthread_t t;
    int s = pthread_create(&t, NULL, threadFunc, NULL);
    //注意线程API中不是-1才表示出错了，只要不是0,都是错
    if(s != 0)  errExitEN(s, "pthread_create");

    s = pthread_join(t, NULL);
    if(s != 0)  errExitEN(s, "pthread_join");

    printf("Main thread: str (%p) = %s\n", str, str);
    exit(EXIT_SUCCESS);

}


