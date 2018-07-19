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
    
    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sfd == -1)   errExit("socket");

    //删除原来可能已存在的unix-socket文件
    if(remove(SV_SOCK_PATH) == -1 && errno != ENOENT)   errExit("remove-%s", SV_SOCK_PATH);

    struct sockaddr_un svaddr, claddr;
    //因为会使用strncpy函数，可能导致sun_path不为\0结尾，所以必须人工清零结构体
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    //只绑定，不需要监听
    if(bind(sfd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) == -1)   errExit("bind");

    int cfd;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    while(true){
        //在这里阻塞，需要接收客户端来源信息
        len = sizeof(struct sockaddr_un);
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *)&claddr, &len);
        if(numBytes == -1)  errExit("recvfrom");

        printf("Server received %ld bytes from %s\n", (long)numBytes, claddr.sun_path);

        //将客户端传来的字符串，都改成大写，并返回客户端
        for(int i = 0; i < numBytes; i++){
            buf[i] = toupper((unsigned char)buf[i]);
        }

        if(sendto(sfd, buf, numBytes, 0, (struct sockaddr *)&claddr, len) != numBytes)  fatal("sendto");
    }
}