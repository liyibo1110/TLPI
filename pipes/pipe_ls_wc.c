#include "../lib/error_functions.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
    
    int pfd[2];

    //创建管道
    if(pipe(pfd) == -1) errExit("pipe");

    switch(fork()){
        case -1:
            errExit("fork");
        case 0:
            //先关闭读入端，只留下输出端
            if(close(pfd[0]) == -1) errExit("close 1");
            if(pfd[1] != STDOUT_FILENO){    //防御性编程
                if(dup2(pfd[1], STDOUT_FILENO) == -1)   errExit("dup2 1");
                if(close(pfd[1]) == -1) errExit("close 2");
            }
            execlp("ls", "ls", (char *)NULL);
            errExit("execlp ls");
        default:
            break;
    }
  
    switch(fork()){
        case -1:
            errExit("fork");
        case 0:
            //先关闭输出端，只留下读入端
            if(close(pfd[1]) == -1) errExit("close 3");
            if(pfd[0] != STDIN_FILENO){    //防御性编程
                if(dup2(pfd[0], STDIN_FILENO) == -1)   errExit("dup2 2");
                if(close(pfd[0]) == -1) errExit("close 4");
            }
            execlp("wc", "wc", "-l", (char *)NULL);
            errExit("execlp wc");
        default:
            break;
    }
    
    if(close(pfd[0]) == -1) errExit("close 5");
    if(close(pfd[1]) == -1) errExit("close 6");
    if(wait(NULL) == -1) errExit("wait 1");
    if(wait(NULL) == -1) errExit("wait 2");
    exit(EXIT_SUCCESS);
    
}