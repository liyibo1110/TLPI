#include "../lib/error_functions.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int istack = 222;   //在stack段中保存，子进程也会拥有自己的副本
    
    switch(vfork()){
        case -1 :
            errExit("fork");
        case 0 : 
            sleep(3);   //子线程睡3秒，测试主线程阻塞情况
            write(STDOUT_FILENO, "Child executing\n", 16);
            istack *= 3;    //在子线程修改变量，会影响到主线程
            _exit(EXIT_SUCCESS);
        default : 
            write(STDOUT_FILENO, "Parent executing\n", 17);
            printf("istack=%d\n", istack);  //值已被主线程修改
            exit(EXIT_SUCCESS);
    }

}
