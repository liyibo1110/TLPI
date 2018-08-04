#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <poll.h> 

int main(int argc, char *argv[]){
    
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s num-pipes [num-writes]\n", argv[0]);
    }

    //pipe总数，动态传入
    int numPipes = atoi(argv[1]);
    
    //需要为pipe调用的结果
    int (*pfds)[2] = calloc(numPipes, sizeof(int[2]));
    if(pfds == NULL)    errExit("calloc"); 

    struct pollfd *pollFd = calloc(numPipes, sizeof(struct pollfd));
    if(pollFd == NULL)  errExit("calloc");

    //创建管道
    for(int i = 0; i < numPipes; i++){
        if(pipe(pfds[i]) == -1) errExit("pipe %d", i);
    }

    //尝试获取第2个参数
    int numWrites = (argc > 2) ? atoi(argv[2]) : 1;

    //初始化种子
    srandom((int)time(NULL));
    int randPipe;
    for(int i = 0; i < numWrites; i++){
        //随机出要写入的pipe序号
        randPipe = random() % numPipes;
        printf("Writing to fd: %3d (read fd: %3d)\n",
                pfds[randPipe][1], pfds[randPipe][0]);
        //开始写入
        if(write(pfds[randPipe][1], "a", 1) == -1)  errExit("write %d", pfds[randPipe][1]);
    }

    //开始监听所有pipe
    for(int i = 0; i < numPipes; i++){
        pollFd[i].fd = pfds[i][0];
        pollFd[i].events = POLLIN;
    }

    int ready = poll(pollFd, numPipes, -1);
    if(ready == -1) errExit("poll");

    printf("poll() returned: %d\n", ready);

    for(int i = 0; i < numPipes; i++){
        if(pollFd[i].revents & POLLIN){
            printf("Readable: %d %3d\n", i, pollFd[i].fd);
        }
    }

    exit(EXIT_SUCCESS);
}
