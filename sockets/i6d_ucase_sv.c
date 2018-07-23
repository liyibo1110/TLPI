#include "./i6d_ucase.h"
#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
    
    int sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(sfd == -1)   errExit("socket");

    struct sockaddr_in6 svaddr, claddr;
    //人工清零结构体各个字段
    memset(&svaddr, 0, sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6;
    svaddr.sin6_addr = in6addr_any;
    svaddr.sin6_port = htons(PORT_NUM); //需要转成网络字节序

    //只绑定，不需要监听
    if(bind(sfd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_in6)) == -1)   errExit("bind");

    int cfd;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    char claddrStr[INET6_ADDRSTRLEN];   //客户端地址
    while(true){
        //在这里阻塞，需要接收客户端来源信息
        len = sizeof(struct sockaddr_in6);
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *)&claddr, &len);
        if(numBytes == -1)  errExit("recvfrom");

        if(inet_ntop(AF_INET6, &claddr.sin6_addr, claddrStr, INET6_ADDRSTRLEN) == NULL){
            printf("Couldn't convert client address to string\n");
        }else{
            printf("Server received %ld bytes from (%s, %u)\n", 
                (long)numBytes, claddrStr, ntohs(claddr.sin6_port));
        }
    
        //将客户端传来的字符串，都改成大写，并返回客户端
        for(int i = 0; i < numBytes; i++){
            buf[i] = toupper((unsigned char)buf[i]);
        }

        if(sendto(sfd, buf, numBytes, 0, (struct sockaddr *)&claddr, len) != numBytes)  fatal("sendto");
    }
}