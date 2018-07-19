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

int main(int argc, char *argv[]){
    
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sfd == -1)   errExit("socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if(connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)    errExit("connect");

    ssize_t numRead;
    char buf[BUF_SIZE];
    //从标准输入读取字符串
    while((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0){
        if(write(sfd, buf, numRead) != numRead) fatal("partial/failed write");
    }

    if(numRead == -1)   errExit("read");
    
    exit(EXIT_SUCCESS);
}