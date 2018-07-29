#include "../lib/error_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

static int cmdNum;  //管道序列中的位置

static void handler(int sig){
    
    //输出进程组首进程的相关信息
    if(getpid() == getpgrp()){
        fprintf(stderr, "Terminal FG process group: %ld\n", (long)tcgetpgrp(STDERR_FILENO));
    }

    fprintf(stderr, "Process %ld (%d) received signal %d (%s)\n",
                (long)getpid(), cmdNum, sig, strsignal(sig));

    //如果受到了TSTP信号，转换成STOP信号，用来停止进程
    if(sig == SIGTSTP)  raise(SIGSTOP);
}

int main(int argc, char *argv[]){

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;

    if(sigaction(SIGINT, &sa, NULL) == -1)  errExit("sigaction");
    if(sigaction(SIGTSTP, &sa, NULL) == -1)  errExit("sigaction");
    if(sigaction(SIGCONT, &sa, NULL) == -1)  errExit("sigaction");

    //检测stdin是否为终端，如果是，说明该进程是管道的第一个进程
    if(isatty(STDIN_FILENO)){
        fprintf(stderr, "Terminal FG process group: %ld\n", (long)tcgetpgrp(STDIN_FILENO));
        fprintf(stderr, "Command    PID    PPID    PGRP    SID\n");
        cmdNum = 0;
    }else{
        //从管道前一个进程获取输入
        if(read(STDIN_FILENO, &cmdNum, sizeof(cmdNum)) <= 0){
            fatal("read got EOF or error");
        }
    }

    cmdNum++;
    fprintf(stderr, "%4d    %5ld %5ld %5ld %5ld\n", cmdNum,
            (long)getpid(), (long)getppid(),
            (long)getpgrp(), (long)getsid(0));

    //如果不是最后一个程序，则输出cmdNum作为下一个程序的输入
    if(!isatty(STDOUT_FILENO)){
        if(write(STDOUT_FILENO, &cmdNum, sizeof(cmdNum)) == -1) errMsg("write");
    }

    while(true){
        pause();
    }
}