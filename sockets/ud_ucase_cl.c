#include "./ud_ucase.h"
#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
    
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s msg...\n", argv[0]);
    }

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sfd == -1)   errExit("socket");

    struct sockaddr_un svaddr, claddr;
    //因为会使用snprintf函数，可能导致sun_path不为\0结尾，所以必须人工清零结构体
    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%ld", (long)getpid());

    //只绑定，不需要监听
    if(bind(sfd, (struct sockaddr *)&claddr, sizeof(struct sockaddr_un)) == -1)   errExit("bind");

    //连接服务端的socket，发送消息
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);
    
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];
    for(int i = 1 ; i < argc; i++){
        msgLen = strlen(argv[i]);
        if(sendto(sfd, argv[i], msgLen, 0, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) != msgLen){
            fatal("sendto");
        }
        //不关心服务端地址
        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if(numBytes == -1)  errExit("recvfrom");

        //输出服务端返回的消息
        printf("Response %d: %.*s\n", i, (int)numBytes, resp);
    }
    remove(claddr.sun_path);
    exit(EXIT_SUCCESS);
}