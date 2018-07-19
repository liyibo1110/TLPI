#include "./us_xfr.h"
#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdbool.h>

#define BACKLOG 5

int main(int argc, char *argv[]){
    
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sfd == -1)   errExit("socket");

    //删除原来可能已存在的unix-socket文件
    if(remove(SV_SOCK_PATH) == -1 && errno != ENOENT)   errExit("remove-%s", SV_SOCK_PATH);

    struct sockaddr_un addr;
    //因为会使用strncpy函数，可能导致sun_path不为\0结尾，所以必须人工清零结构体
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    //绑定并开始监听
    if(bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)   errExit("bind");
    if(listen(sfd, BACKLOG) == -1)  errExit("listen");

    int cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];
    while(true){
        //在这里阻塞，不需要接收客户端来源信息
        cfd = accept(sfd, NULL, NULL);
        if(cfd == -1)   errExit("accept");
        while((numRead = read(cfd, buf, BUF_SIZE)) > 0){
            if(write(STDOUT_FILENO, buf, numRead) != numRead)   fatal("partial/failed write");
        }
        if(numRead == -1)   errExit("read");
        //还得关闭请求而来的描述符
        if(close(cfd) == -1)    errExit("close");
    }
}