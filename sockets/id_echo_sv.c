#include "./inet_sockets.h"
#include "./id_echo.h"
#include "../daemons/become_daemon.h"
#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <syslog.h>
int main(int argc, char *argv[]){

    //变成daemon进程
    if(becomeDaemon(0) == -1)   errExit("becomeDaemon");

    int sfd = inetBind(SERVICE, SOCK_DGRAM, NULL);
    if(sfd == -1){
        syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //这就行了，开始监听
    socklen_t len;
    ssize_t numRead;
    struct sockaddr_storage claddr;
    char buf[BUF_SIZE]; //请求来的内容
    char addrStr[IS_ADDR_STR_LEN];  //请求来的地址
    while(true){
        len = sizeof(struct sockaddr_storage);
        numRead = recvfrom(sfd, buf, BUF_SIZE, 0,
                            (struct sockaddr *)&claddr, &len);
        if(numRead == -1)   errExit("recvfrom");
        //再发回去
        if(sendto(sfd, buf, numRead, 0,
                    (struct sockaddr *)&claddr, len) != numRead){
            syslog(LOG_WARNING, "Error echoing response to %s (%s)", 
                    inetAddressStr((struct sockaddr *)&claddr, len, addrStr, IS_ADDR_STR_LEN),
                    strerror(errno));
        }
    }
}