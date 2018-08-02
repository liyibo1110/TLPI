#include "./inet_sockets.h"
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
#include <signal.h>
#include <sys/wait.h>

#define SERVICE "echo"
#define BUF_SIZE 4096

int main(int argc, char *argv[]){

    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s host\n", argv[0]);
    }

    int sfd = inetConnect(argv[1], "echo", SOCK_STREAM);
    if(sfd == -1)   errExit("inetConnect");

    ssize_t numRead;
    char buf[BUF_SIZE];
    switch(fork()){
        case -1 :
            errExit("fork");
        case 0 :
            while(true){
                numRead = read(sfd, buf, BUF_SIZE);
                if(numRead <= 0)    break;  //遇到EOF了（当父进程调用shutdown，服务端就会发送EOF过来了），或者发生错误，直接退出
                printf("%.*s", (int)numRead, buf);
            }
            exit(EXIT_SUCCESS);
        default :
            while(true){
                numRead = read(STDIN_FILENO, buf, BUF_SIZE);
                if(numRead <= 0)    break;  //遇到EOF了，或者发生错误，直接退出
                if(write(sfd, buf, numRead) != numRead) fatal("write() failed");
            }
            //关闭写操作
            if(shutdown(sfd, SHUT_WR) == -1)    errExit("shutdown");
            exit(EXIT_SUCCESS);
    }
}