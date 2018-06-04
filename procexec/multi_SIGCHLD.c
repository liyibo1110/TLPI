#include "../lib/error_functions.h"
#include "../time/curr_time.h"
#include "./print_wait_status.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

extern int errno;
static volatile int numLiveChildren = 0;   

static void sigchldHandler(int sig){
    
    int savedErrno = errno; //暂存全局变量errno的初始值

    printf("%s handler: Caught SIGCHLD\n", currTime("%T"));

    int status;
    pid_t childPid;

    while((childPid = waitpid(-1, &status, WNOHANG)) > 0){
        printf("%s handler: Reaped child %ld - ", currTime("%T"), (long)childPid);
        printWaitStatus(NULL, status);
        numLiveChildren--;
    }

    if(childPid == -1 && errno != ECHILD)   errMsg("waitpid");
    sleep(5);   //拖延处理时间
    printf("%s handler: returning\n", currTime("%T"));
    errno = savedErrno; //最后还原回去
}

int main(int argc, char *argv[]){

   
}
