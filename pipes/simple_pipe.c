#include "../lib/error_functions.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BUF_SIZE 10

int main(int argc, char *argv[]){
    int pfd[2];
    char buf[BUF_SIZE];
    ssize_t numRead;
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s string\n", argv[0]);
    }

    //创建管道
    if(pipe(pfd) == -1) errExit("pipe");

    switch(fork()){
        case -1:
            errExit("fork");
        case 0:
            //先关闭输出端，只留下读入端
            if(close(pfd[1]) == -1) errExit("close - child");
            while(true){
                //从读入端开始尝试读取
                numRead = read(pfd[0], buf, BUF_SIZE);
                if(numRead == -1) errExit("read");
                if(numRead == 0) break; //说明读完了
                //都没问题则写入输出端
                if(write(STDOUT_FILENO, buf, numRead) != numRead){
                    fatal("child - partial/failed write");
                }
            }
            //还要输入换行
            write(STDOUT_FILENO, "\n", 1);
            if(close(pfd[0] == -1)) errExit("close");
            _exit(EXIT_SUCCESS);
        default:
            //先关闭读入端，只留下输出端
            if(close(pfd[0]) == -1) errExit("close - parent");
            //写入管道
            if(write(pfd[1], argv[1], strlen(argv[1])) != strlen(argv[1])){
                fatal("parent - partial/failed write");
            }
            if(close(pfd[1] == -1)) errExit("close");
            wait(NULL);
            exit(EXIT_SUCCESS);
    }
}