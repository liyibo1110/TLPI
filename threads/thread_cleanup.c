#include "../lib/error_functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

static pthread_cond_t connd = PTHREAD_COND_INITIALIZER;
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
    
    return NULL;
}

int main(int argc, char *argv[]){

    exit(EXIT_SUCCESS);
}


