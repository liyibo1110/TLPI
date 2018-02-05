#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "../lib/error_functions.h"

static void sigHandler(int sig){
    printf("Ouch!\n");  //并不安全的写法
}

int main(void){
    //注册signal
    if(signal(SIGINT, sigHandler) == SIG_ERR){
        errExit("signal");
    }

    //每隔3秒循环一次
    for (int i = 0;;i++){
        printf("%d\n", i);
        sleep(3);
    }
}