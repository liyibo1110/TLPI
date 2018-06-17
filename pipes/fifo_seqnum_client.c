#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "./fifo_seqnum.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

extern int errno;
static char clientFifo[CLIENT_FIFO_NAME_LEN];

//main执行完毕，要删除相关的fifo文件
static void removeFifo(void){
    unlink(clientFifo);
}

int main(int argc, char *argv[]){
   
    struct request req;
    struct response res;
    if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [seq-len...]\n", argv[0]);
    }

    umask(0);
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)getpid());
    if(mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        errExit("mkfifo %s", clientFifo);
    }

    //注册退出回调函数
    if(atexit(removeFifo) != 0) errExit("atexit");

    //构造request
    req.pid = getpid();
    req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

    int serverFd = open(SERVER_FIFO, O_WRONLY);
    if(serverFd == -1)  errExit("open %s", SERVER_FIFO);

    //发送
    if(write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)){
        fatal("Can't write to server");
    }

    //等待回复并读取处理
    int clientFd = open(clientFifo, O_RDONLY);
    if(clientFd == -1)  errExit("open %s", clientFifo);

    if(read(clientFd, &res, sizeof(struct response)) != sizeof(struct response)){
        fatal("Can't read response from server");
    }

    printf("%d\n", res.seqNum);
    exit(EXIT_SUCCESS);
}