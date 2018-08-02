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

static void grimReaper(int sig){

    int savedErrno = errno;
    //收尸
    while(waitpid(-1, NULL,WNOHANG) > 0)    continue;
    errno = savedErrno;
}

static void handleRequest(int cfd){
    char buf[BUF_SIZE];
    ssize_t numRead;
    while((numRead = read(cfd, buf, BUF_SIZE)) > 0){
        if(write(cfd, buf, numRead) != numRead){
            syslog(LOG_ERR, "write() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if(numRead == -1){
        syslog(LOG_ERR, "Error from read(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]){

    if(becomeDaemon(0) == -1)   errExit("becomeDaemon");
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if(sigaction(SIGCHLD, &sa, NULL) == -1){
        syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int lfd = inetListen(SERVICE, 10, NULL);
    if(lfd == -1){
        syslog(LOG_ERR, "Could not create server socket (%s)",  strerror(errno));
        exit(EXIT_FAILURE);
    }

    int cfd;
    while(true){
        //不需要知道客户端的情况
        cfd = accept(lfd, NULL, NULL);
        if(cfd == -1){
            syslog(LOG_ERR, "Failure in accept(): (%s)",  strerror(errno));
            exit(EXIT_FAILURE);
        }

        switch(fork()){
            case -1 :
                syslog(LOG_ERR, "Can't create child (%s)", strerror(errno));
                close(cfd);
                break;
            case 0 :
                close(lfd); //立即关闭冗余的监听描述符副本
                handleRequest(cfd);
                _exit(EXIT_SUCCESS);    //子进程直接要结束
            default : 
                close(cfd); //主进程立即关闭客户端描述符副本
                break;

        }
    }
}