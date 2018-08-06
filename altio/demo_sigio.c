#include "../lib/error_functions.h"
#include "../tty/tty_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <stdbool.h>

static volatile sig_atomic_t gotSigio = 0;

static void sigioHandler(int sig){
    gotSigio = 1;   //只是置位，怎么处理I/O，是主线程负责的
}

int main(int argc, char *argv[]){
    
    //为SIGIO建立处理
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigioHandler;
    if(sigaction(SIGIO, &sa, NULL) == -1)   errExit("sigaction");

    //本例子只会处理标准输入这1个描述符
    //改变owner属性，改成当前进程
    if(fcntl(STDIN_FILENO, F_SETOWN, getpid()) == -1)   errExit("fcntl(F_SETOWN)");
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    if(fcntl(STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1)    errExit("fcntl(F_SETFL)");

    //将终端模式弄成cbreak
    struct termios origTermios;
    if(ttySetCbreak(STDIN_FILENO, &origTermios) == -1)  errExit("ttySetCbreak");

    int count = 0;
    bool done = false;
    char ch;
    while(!done){
        //每次都进来拖延一会儿
        for(int i = 0; i < 100000000; i++){
            continue;
        }
        if(gotSigio){
            gotSigio = 0;   //还原回去
            //有I/O准备就绪了，因为是边缘触发，所以要尽量多读
            while(read(STDIN_FILENO, &ch, 1) > 0 && !done){
                printf("count=%d; read %c\n", count, ch);
                //如果输入#，程序结束
                done = (ch == '#');
            }
        }
        count++;
    }

    //还原终端配置
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1) errExit("tcsetattr");

    exit(EXIT_SUCCESS);
}
