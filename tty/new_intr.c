#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <termios.h>

int main(int argc, char *argv[]){
    
    if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [intr-char]\n", argv[0]);
    }

    int intrChar;
    if(argc == 1){  //没传参数，则禁止INTR键位失效
        intrChar = fpathconf(STDIN_FILENO, _PC_VDISABLE);
        if(intrChar == -1)  errExit("Could't determine VDISABLE");
    }else if(isdigit((unsigned char)argv[1][0])){
        //支持数字
        intrChar = strtoul(argv[1], NULL, 0);
    }else{
        intrChar = argv[1][0];
    }

    //开始设置
    struct termios tp;
    if(tcgetattr(STDIN_FILENO, &tp) == -1)  errExit("tcgetattr");
    tp.c_cc[VINTR] = intrChar;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1)   errExit("tcsetattr");

    exit(EXIT_SUCCESS);
}
