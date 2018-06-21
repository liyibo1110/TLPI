#include "svmsg_file.h"
#include "../lib/error_functions.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/msg.h>

extern int errno;

//SIGCHLD信号处理器，当子进程执行完毕，会调用wait进行关闭（也考虑到了信号的并发合并特征，只能用waitpid循环关闭所有）
static void grimReaper(int sig){
    int savedErrno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0){
        continue;
    }   
    errno = savedErrno;
}

static void serveRequest(const struct requestMsg *req){
    struct responseMsg res;
    //尝试打开客户端指定的文件
    int fd = open(req->pathname, O_RDONLY);
    if(fd == -1){
        res.mtype = RES_MT_FAILURE;
        snprintf(res.data, sizeof(res.data), "%s", "Couldn't open");
        msgsnd(req->clientId, &res, strlen(res.data) + 1, 0);
        exit(EXIT_FAILURE);
    }

    ssize_t numRead;
    while((numRead = read(fd, res.data, RES_MSG_SIZE)) > 0){
        if(msgsnd(req->clientId, &res, numRead, 0) == -1){
            break;
        }
    }
    //最后发送结束type
    res.mtype = RES_MT_END;
    msgsnd(req->clientId, &res, 0, 0);
}

int main(int argc, char *argv[]){
    struct requestMsg req;
    struct sigaction sa;
    int serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
    if(serverId == -1)  errEixt("msgget");

    //建立SIGCHLD信号处理器
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;   //允许主进程里msgrcv系统调用阻塞时，同时被SIGCHLD信号中断后，可自动恢复
    sa.sa_handler = grimReaper;
    if(sigaction(SIGCHLD, &sa, NULL) == -1) errExit("sigaction");

    pid_t pid;
    ssize_t msgLen;
    while(true){
        msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);
        if(msgLen == -1){
            //如果返回-1,可能是被SIGCHLD信号中断，这属于正常现象
            if(errno == EINTR){
                continue;
            }
            errMsg("msgrcv");
            break;
        }

        pid = fork();
        if(pid == -1){
            errMsg("fork");
            break;
        }

        if(pid == 0){
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }
    }

    //到了这里说明msgrc或者fork出现错误，删除消息队列并退出
    if(msgctl(serverId, IPC_RMID, NULL) == -1)  errExit("msgctl");
    exit(EXIT_SUCCESS);
}

   

