#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../lib/error_functions.h"

static void sigHandler(int sig){
    static int count = 0;

    if(sig == SIGINT){
        count++;
        printf("Caught SIGINT (%d)\n", count);
        return;
    }
    
    printf("Caught SIGQUIT - this's all folks!\n");  //并不安全的写法
    exit(EXIT_SUCCESS);
}

int main(void){
    //注册signal
    if(signal(SIGINT, sigHandler) == SIG_ERR){
        errExit("signal");
    }
    if(signal(SIGQUIT, sigHandler) == SIG_ERR){
        errExit("signal");
    }
    //无限循环
    while(1){
        pause();
    }
}