#include "./inet_sockets.h"
#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>

int main(int argc, char *argv[]){

    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s service\n", argv[0]);
    }

    socklen_t len;
    int listenFd = inetListen(argv[1], 5, &len);
    if(listenFd == -1)  errExit("inetListen");

    //直接在这里连接上面的服务
    int connFd = inetConnect(NULL, argv[1], SOCK_STREAM);
    if(connFd == -1)    errExit("inetConnect");

    int acceptFd = accept(listenFd, NULL, NULL);
    if(acceptFd == -1)  errExit("accept");

    //保存获取到的socket信息
    void *addr = malloc(len);
    if(addr == NULL)    errExit("malloc");

    char addrStr[IS_ADDR_STR_LEN];
    if(getsockname(connFd, addr, &len) == -1)   errExit("getsockname");
    printf("getsockname(connFd):    %s\n",  inetAddressStr(addr, len, addrStr, IS_ADDR_STR_LEN));

    if(getsockname(acceptFd, addr, &len) == -1)   errExit("getsockname");
    printf("getsockname(acceptFd):    %s\n",  inetAddressStr(addr, len, addrStr, IS_ADDR_STR_LEN));

    if(getpeername(connFd, addr, &len) == -1)   errExit("getpeername");
    printf("getpeername(connFd):    %s\n",  inetAddressStr(addr, len, addrStr, IS_ADDR_STR_LEN));

    if(getpeername(acceptFd, addr, &len) == -1)   errExit("getpeername");
    printf("getpeername(acceptFd):    %s\n",  inetAddressStr(addr, len, addrStr, IS_ADDR_STR_LEN));

    sleep(30);  //暂停运行，可以用netstat来验证获取到信息是否正确
    exit(EXIT_SUCCESS);
}