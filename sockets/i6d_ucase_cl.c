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
    
    if(argc < 3 || strcmp(argv[1], "--help") == 0){
        usageErr("%s host-address msg...\n", argv[0]);
    }

    int sfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(sfd == -1)   errExit("socket");

    struct sockaddr_in6 svaddr;
    //必须人工清零结构体
    memset(&svaddr, 0, sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6;
    svaddr.sin6_port = htons(PORT_NUM);
    if(inet_pton(AF_INET6, argv[1], &svaddr.sin6_addr) <= 0){
        fatal("inet_pton failed for address '%s'", argv[1]);
    }

    //连接服务端的socket，发送消息
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];
    for(int i = 2 ; i < argc; i++){
        msgLen = strlen(argv[i]);
        if(sendto(sfd, argv[i], msgLen, 0, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_in6)) != msgLen){
            fatal("sendto");
        }
        //不关心服务端地址
        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if(numBytes == -1)  errExit("recvfrom");

        //输出服务端返回的消息
        printf("Response %d: %.*s\n", i - 1, (int)numBytes, resp);
    }
    exit(EXIT_SUCCESS);
}