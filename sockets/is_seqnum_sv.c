#define _BSD_SOURCE //为了使用NI_MAXHOST和NI_MAXSERV常量
#include "./is_seqnum.h"
#include "./read_line.h"
#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>

#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
#define BACKLOG 50

int main(int argc, char *argv[]){
    
    if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [init-seq-num]\n", argv[0]);
    }

    //服务端维护的当前序列值
    uint32_t seqNum = (argc > 1) ? atoi(argv[1]) : 1;
    //服务端必须忽略PIPE信号，以至于不受因客户端关闭而造成的反伤
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) errExit("signal");

    struct addrinfo *result, *rp;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;    //允许IPv4或IPv6
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    if(getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0)   errExit("getaddrinfo");

    int optval = 1;
    int lfd;    //监听描述符
    //getaddrinfo会返回不止1个可绑定的地址，所以需要循环依次尝试
    for(rp = result; rp != NULL; rp = rp->ai_next){
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(lfd == -1)   continue;   //如果失败，则尝试下一个地址
        //需要添加SO_REUSEADDR功能，使得服务器关闭后，不会遇到再等待2分钟的TIME_WAIT残留问题
        if(setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)    errExit("setsockopt");
        if(bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        //到这里，说明bind失败，需要关闭监听描述符，并开始下一轮尝试
        close(lfd);
    }

    if(rp == NULL)  fatal("Could not bind socket to any address");

    if(listen(lfd, BACKLOG) == -1)  errExit("listen");

    //result没用了，需要释放
    freeaddrinfo(result);

    socklen_t addrlen; 
    int cfd;
    struct sockaddr_storage claddr; //兼容各种形式的客户端地址信息
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    char addrStr[ADDRSTRLEN];   //存放客户端的IP和端口（有足够的大小）
    char reqLenStr[INT_LEN];    //客户端请求信息
    int reqLen; //客户端请求信息
    char seqNumStr[INT_LEN];    //响应信息
    while(true){
        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
        if(cfd == -1){
            //继续监听
            errMsg("accept");
            continue;
        }
        //获取客户端的可读地址，注意addrlen已经变成accept更改过的长度了
        if(getnameinfo((struct sockaddr *)&claddr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        }else{
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        }
        printf("Connection from %s\n", addrStr);

        //开始正式读取客户端的请求信息，并返回序列结果信息
        //将客户端请求信息统一弄成字符串，直接消除不同主机系统的兼容性问题
        if(readLine(cfd, reqLenStr, INT_LEN) <= 0){
            close(cfd);
            continue;
        }

        printf("server readLine over...\n");

        reqLen = atoi(reqLenStr);
        //传来负数直接不管
        if(reqLen <= 0){
            close(cfd);
            continue;
        }

        snprintf(seqNumStr, INT_LEN, "%d\n", seqNum);
        //输出响应，注意也是要转成字符串
        printf("server ready to send");
        if(write(cfd, &seqNumStr, strlen(seqNumStr)) != strlen(seqNumStr)){
            fprintf(stderr, "Error on write");
        }
        printf("server send over...");
        //生成新的序列
        seqNum += reqLen;
        if(close(cfd) == -1)    errMsg("close");
    }
}