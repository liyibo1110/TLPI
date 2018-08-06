#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAX_BUF 1000
#define MAX_EVENTS 5    //每次最多返回5个就绪的可用描述符

int main(int argc, char *argv[]){
    
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s file...\n", argv[0]);
    }

    int epfd = epoll_create(argc - 1);
    if(epfd == -1)  errExit("epoll_create");

    int fd;
    struct epoll_event ev;  //可以复用
    for(int i = 1; i < argc; i++){
        fd = open(argv[i], O_RDONLY);
        if(fd == -1)    errExit("open");
        printf("Opened \"%s\" on fd %d\n", argv[i], fd);
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)   errExit("epoll_ctl");
    }   

    int numOpenFds = argc - 1;  //总监控的描述符数目
    int ready;
    struct epoll_event evlist[MAX_EVENTS];
    int s;
    char buf[MAX_BUF];
    while(numOpenFds > 0){
        printf("About to epoll_wait()\n");
        ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
        if(ready == -1){
            if(errno == EINTR){
                continue;
            }else{
                errExit("epoll_wait");
            }
        }
        printf("Ready: %d\n", ready);
        //处理就绪的描述符们
        for(int i = 0; i < ready; i++){
            printf("    fd=%d; events: %s%s%s\n", evlist[i].data.fd,
                    (evlist[i].events & EPOLLIN) ? "EPOLLIN " : "", 
                    (evlist[i].events & EPOLLHUP) ? "EPOLLHUP" : "",
                    (evlist[i].events & EPOLLERR) ? "EPOLLERR" : "");
            if(evlist[i].events & EPOLLIN){
                s = read(evlist[i].data.fd, buf, MAX_BUF);
                if(s == -1) errExit("read");
                printf("    read %d bytes: %.*s\n", s, s, buf);
            }else if(evlist[i].events & (EPOLLHUP | EPOLLERR)){
                //如果FIFO对端关闭，或者终端挂起，就会进入这里，需要关闭描述符
                printf("    closing fd %d\n", evlist[i].data.fd);
                if(close(evlist[i].data.fd) == -1)  errExit("close");
                numOpenFds--;
            }
        }
    }
    //当所有就绪的描述符关闭或者挂起，则退出程序
    printf("All file descriptors closed; bye\n");
    exit(EXIT_SUCCESS);
}
