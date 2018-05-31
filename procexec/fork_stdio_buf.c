#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    printf("Hello world\n");    //如果程序被重定向到文件里，则printf变成块缓冲，并不会立刻flush，会造成fork之后的分裂并延迟到exit后输出
    write(STDOUT_FILENO, "Ciao\n", 5);

    fflush(stdout); //避免了上述的问题

    if(fork() == -1)    errExit("fork");

    //也可以在子进程调用_exit函数来退出，这也是多进程程序的一个准则，即子进程都使用_exit退出

    //父子进程都什么都不做了

    exit(EXIT_SUCCESS);
} 
