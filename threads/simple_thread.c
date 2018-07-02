#include "../lib/error_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

/**
 * 线程要执行的函数
 */ 
static void *threadFunc(void *arg){
    char *s = (char *)arg;
    printf("%s", s);
    return (void *)strlen(s);   //返回字符串长度
}

int main(int argc, char *argv[]){

	pthread_t t1;
    int s = pthread_create(&t1, NULL, threadFunc, "Hello world\n");
    //注意线程API中不是-1才表示出错了，只要不是0,都是错
    if(s != 0)  errExitEn(s, "pthread_create");

    printf("Message from main()\n");
    void *res;
    s = pthread_join(t1, &res); //注意res本身是指针，但还是要取地址，因为要改变自身
    if(s != 0)  errExitEn(s, "pthread_join");
    printf("Thread returned %ld\n", (long)res);
    exit(EXIT_SUCCESS);

}


