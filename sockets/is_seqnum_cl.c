#define _BSD_SOURCE
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
#include <stdbool.h>

int main(int argc, char *argv[]){
    
    if(argc < 2 && strcmp(argv[1], "--help") == 0){
        usageErr("%s server-host [sequence-len]\n", argv[0]);
    }

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;    //允许IPv4或IPv6
    hints.ai_flags = AI_NUMERICSERV;
    if(getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0)   errExit("getaddrinfo");

    int cfd;    //客户端描述符
    //getaddrinfo会返回不止1个可绑定的地址，所以需要循环依次尝试
    for(rp = result; rp != NULL; rp = rp->ai_next){
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(cfd == -1)   continue;   //如果失败，则尝试下一个地址
        if(connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1) break;
        //到这里，说明connect失败，需要客户端描述符，并开始下一轮尝试
        close(cfd);
    }

    if(rp == NULL)  fatal("Could not connect socket to any address");

    //result没用了，需要释放
    freeaddrinfo(result);

    //开始发送请求消息
    char *reqLenStr = (argc > 2) ? argv[2] : "1";
    //注意是没有'\0'的
    printf("ready to send...\n");
    if(write(cfd, reqLenStr, strlen(reqLenStr)) != strlen(reqLenStr)){
        fatal("Partial/failed write (reqLenStr)");
    }
    if(write(cfd, "\n", 1) != 1){
        fatal("Partial/failed write (newline)");
    }
    printf("send over...\n");
   
    char seqNumStr[INT_LEN];
    printf("ready to readLine\n");
    ssize_t numRead = readLine(cfd, seqNumStr, INT_LEN);
    printf("readLine over, numRead=%ld\n", (long)numRead);
    if(numRead == -1)   errExit("readLine");
    if(numRead == 0)    fatal("Unexpected EOF from server");
    //注意从服务端返回的seqNumStr是带换行符的
    printf("Sequence number: %s", seqNumStr);
    exit(EXIT_SUCCESS);
}