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
int main(int argc, char *argv[]){

    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s host msg...\n", argv[0]);
    }

    int sfd = inetConnect(argv[1], SERVICE, SOCK_DGRAM);
    if(sfd == -1){
        fatal("Could not connect to server socket");
    }

    //开始请求
    size_t len;
    ssize_t numRead;
    char buf[BUF_SIZE];
    for(int i = 2; i < argc; i++){
        len = strlen(argv[i]);
        if(write(sfd, argv[i], len) != len){
            fatal("partial/failed write");
        }
        numRead = read(sfd, buf, BUF_SIZE);
        if(numRead == -1)   errExit("read");
        printf("[%ld bytes] %.*s\n", (long)numRead, (int)numRead, buf);
    }
    exit(EXIT_SUCCESS);
}