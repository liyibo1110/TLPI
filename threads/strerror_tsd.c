#define _GNU_SOURCE
#include "../lib/error_functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_ERROR_LEN 256
static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorkey;   //定义全局的key

static void destructor(void *buf){
    free(buf);
}

/**
 * 定义初始化函数，只有第一个线程会进入执行
 */ 
static void createKey(void){
    int s = pthread_key_create(&strerrorkey, destructor);
    if(s != 0)  errExitEN(s, "pthread_key_create");
}

/**
 * 由错误编号返回错误信息（线程安全版）
 */ 
char *strerror(int err){
    
    int s = pthread_once(&once, createKey);
    if(s != 0)  errExitEN(s, "pthread_once");
    
    char *buf = pthread_getspecific(strerrorkey);
    if(buf == NULL){
        buf = malloc(MAX_ERROR_LEN);
        if(buf == NULL) errExit("malloc");
        s = pthread_setspecific(strerrorkey, buf);
        if(s != 0)  errExitEN(s, "pthread_setspecific");
    } 

    if(err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL){
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    }else{
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';
    }
    return buf;
}