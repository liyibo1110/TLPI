#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

#define BUF_SIZE 100

int main(int argc, char *argv[]){
    
    struct termios tp, save;
    if(tcgetattr(STDIN_FILENO, &tp) == -1)  errExit("tcgetattr");
    save = tp;  //暂存原来的配置
    tp.c_lflag &= ~ECHO;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1)   errExit("tcsetattr");

    //已关闭了ECHO回显功能
    printf("Enter text: ");
    fflush(stdout); //没有换行，所以必须强制输出

    char buf[BUF_SIZE];
    if(fgets(buf, BUF_SIZE, stdin) == NULL){
        printf("Got end-of-file/error on fgets()\n");
    }else{
        printf("\nRead: %s", buf);
    }

    //恢复回显功能
    if(tcsetattr(STDIN_FILENO, TCSANOW, &save) == -1)   errExit("tcsetattr");
    exit(EXIT_SUCCESS);
}
