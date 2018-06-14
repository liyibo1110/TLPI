#include "../lib/error_functions.h"
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

int main(int argc, char *argv[]){
    struct request req;
    struct response res;
    int seqNum = 0; //初始化要返回的序列值

    //尝试创建server读取用的FIFO文件
    umask(0);
    //允许已存在
    if(mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        errExit("mkfifo %s", SERVER_FIFO);
    }

    int serverFd = open(SERVER_FIFO, O_RDONLY); //当作普通文件打开，到这里会阻塞，等待client端向FIFO写入
    int dummyFd = open(SERVER_FIFO, O_WRONLY);  //用来占位，否则所有的client都关闭了写入描述符，这个文件将返回EOF
    if(dummyFd == -1)   errExit("open %s", SERVER_FIFO);

    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) errExit("signal");  //防止向客户端FIFO写入时，收到SIGPIPE信号（默认杀死进程）而是会从write函数中得到EPIPE错误

    //开始读取request并返回response
    while(true){
        //检测获取数据完整性，必须是一个request结构的大小，否则丢弃
        if(read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)){
            fprintf(stderr, "Error reading request; discarding\n");
            continue;
        }

        //开启相应client的FIFO
    }
}