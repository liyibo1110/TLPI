#include "../lib/error_functions.h"
#include "./tty_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>

//原来的配置
static struct termios userTermios;

/**
 *  默认处理器，只负责还原配置并退出 
 */
static void handler(int sig){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1)  errExit("tcsetattr");
    _exit(EXIT_SUCCESS);
}

/**
 * TSTP信号的处理器，因为TSTP信号默认并不会停止进程，要点是：
 * 1.还原默认终端配置（即为挂起前自定义动作的时机）
 * 2.TSTP信号处理还原，并重发信号，这时因为还在信号处理器里，所以会被阻塞
 * 3.立即解除TSTP信号的阻塞，这时默认信号处理机制将生成，进程会被挂起，等待CONT信号
 * 4.再次阻塞TSTP信号，防止重新注册以后返回之前，又收到TSTP，形成处理器递归调用的情况
 */ 
static void tstpHandler(int sig){

    struct termios ourTermios;
    int savedErrno = errno;

    //保存特殊模式中的终端配置
    if(tcgetattr(STDIN_FILENO, &ourTermios) == -1)  errExit("tcgetattr");
    //先还原原始配置
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1)  errExit("tcsetattr");

    //以上为TSTP信号默认执行挂起之前，加入的动作，现在开始还原TSTP的处理
    if(signal(SIGTSTP, SIG_DFL) == SIG_ERR) errExit("signal");
    raise(SIGTSTP); //重发

    sigset_t tstpMask, prevMask;
    struct sigaction sa;
    sigemptyset(&tstpMask);
    sigaddset(&tstpMask, SIGTSTP);
    //暂时移除TSTP信号的阻塞
    if(sigprocmask(SIG_UNBLOCK, &tstpMask, &prevMask) == -1)    errExit("sigprocmask");
    //TSTP默认处理之后，收到了CONT信号，则会从此处继续
    if(sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)    errExit("sigprocmask");
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = tstpHandler;
    if(sigaction(SIGTSTP, &sa, NULL) == -1) errExit("sigaction");

    //挂起期间，用户可能又修改了终端配置，所以要尝试重新获取
    if(tcgetattr(STDIN_FILENO, &userTermios) == -1) errExit("tcgetattr");
    //获取特殊模式的原始配置
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &ourTermios) == -1)  errExit("tcsetattr");

    errno = savedErrno;
}

int main(int argc, char *argv[]){

    struct sigaction sa, prev;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if(argc > 1){   //进入cbreak模式
        if(ttySetCbreak(STDIN_FILENO, &userTermios) == -1)  errExit("ttySetCbreak");
        //注册默认恢复原始的处理器
        sa.sa_handler = handler;
        //处理SIGQUIT信号，如果之前没有屏蔽，则发送信号
        if(sigaction(SIGQUIT, NULL, &prev) == -1)   errExit("sigaction");
        if(prev.sa_handler != SIG_IGN){
            if(sigaction(SIGQUIT, &sa, NULL) == -1)   errExit("sigaction");
        }
        //处理SIGINT信号，如果之前没有屏蔽，则发送信号
        if(sigaction(SIGINT, NULL, &prev) == -1)   errExit("sigaction");
        if(prev.sa_handler != SIG_IGN){
            if(sigaction(SIGINT, &sa, NULL) == -1)   errExit("sigaction");
        }
        sa.sa_handler = tstpHandler;
        //处理SIGTSTP信号，如果之前没有屏蔽，则发送信号
        if(sigaction(SIGTSTP, NULL, &prev) == -1)   errExit("sigaction");
        if(prev.sa_handler != SIG_IGN){
            if(sigaction(SIGTSTP, &sa, NULL) == -1)   errExit("sigaction");
        }
    }else{  //进入原始模式
        if(ttySetRaw(STDIN_FILENO, &userTermios) == -1)  errExit("ttySetRaw");
    }

    //不管什么模式，都要响应TERM信号
    sa.sa_handler = handler;
    if(sigaction(SIGTERM, &sa, NULL) == -1)   errExit("sigaction");

    setbuf(stdout, NULL);

    char ch;
    ssize_t n;
    while(true){
        n = read(STDIN_FILENO, &ch, 1);
        if(n == -1){
            errMsg("read");
            break;
        }
        if(n == 0)  break;  //终端断开才会返回0
        if(isalpha((unsigned char)ch)){ //字母转成小写再输出
            putchar(tolower((unsigned char)ch));
        }else if(ch == '\n' || ch == '\r'){ //换行符则直接输出
            putchar(ch);
        }else if(iscntrl((unsigned char)ch)){   //控制字符转成可读形式
            printf("^%c", ch ^ 64);
        }else{
            putchar('*');
        }
        if(ch == 'q')   break;
        //最终要恢复终端配置
        if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1)  errExit("tcsetattr");
        exit(EXIT_SUCCESS);
    }
}