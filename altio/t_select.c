#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h> 

static void usageError(const char *progName){
    fprintf(stderr, "Usage: %s {timeout|-} fd-num[rw]...\n", progName);
    fprintf(stderr, "   - means infinite timeout\n");
    fprintf(stderr, "   r = monitor for read\n");
    fprintf(stderr, "   w = monitor for write\n\n");
    fprintf(stderr, "   e.g.: %s - 0rw 1w\n", progName);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
    
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageError(argv[0]);
    }

    struct timeval timeout;
    struct timeval *pto;

    if(strcmp(argv[1], "-") == 0){
        pto = NULL; //无限等待
    }else{
        timeout.tv_sec = strtol(argv[1], NULL, 10);
        timeout.tv_usec = 0;
        pto = &timeout;
    }

    int nfds = 0;
    fd_set readfds, writefds;
    //必须初始化
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    int numRead;    //读入的后续参数个数
    int fd;
    char buf[10];    //保存rw足够了
    for(int i = 2; i < argc; i++){
        //开始处理后面的参数
        numRead = sscanf(argv[i], "%d%2[rw]", &fd, buf);
        if(numRead != 2)    usageError(argv[0]);
        //不能大于系统限制的1024（可以改）
        if(fd >= FD_SETSIZE)    cmdLineErr("file descriptor exceeds limit (%d)\n", FD_SETSIZE);
        if(fd >= nfds)  nfds = fd + 1;  //保证nfds是所有fd的最大值+1
        //放入相应的描述符集合中
        if(strchr(buf, 'r') != NULL)    FD_SET(fd, &readfds);
        if(strchr(buf, 'w') != NULL)    FD_SET(fd, &writefds);
    }
    
    int ready = select(nfds, &readfds, &writefds, NULL, pto);
    if(ready == -1) errExit("select");
    printf("ready = %d\n", ready);

    //开始人肉查看是哪个fd有结果了
    for(fd = 0; fd < nfds; fd++){
        printf("%d: %s%s\n", fd, FD_ISSET(fd, &readfds) ? "r" : "", 
                                 FD_ISSET(fd, &writefds) ? "w" : "");
    }

    if(pto != NULL){
        printf("timeout after select(): %ld.%03ld\n",
                (long)timeout.tv_sec, (long)timeout.tv_usec / 10000);
    }

    exit(EXIT_SUCCESS);
}
