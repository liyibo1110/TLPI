#include "../lib/error_functions.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
static int idata = 111; //在data段中保存，子进程也会拥有自己的副本

int main(int argc, char *argv[]){
    int istack = 222;   //在stack段中保存，子进程也会拥有自己的副本
    pid_t childPid;
    
    switch(childPid = fork()){
        case -1 :
            errExit("fork");
        case 0 : 
            idata *= 3;
            istack *= 3;
            break;            
        default : 
            sleep(3);   //主线程睡3秒，让子进程先完成（粗糙的方法）
            break;
    }

    //父子线程都会参与输出
    printf("PID=%ld %s idata=%d istack=%d\n", (long)getpid(), 
            (childPid == 0) ? "(child)" : "(parent)", idata, istack);
    exit(EXIT_SUCCESS);
}
