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

static int clientId;    //文件内全局变量

static void removeQueue(void){
    if(msgctl(clientId, IPC_RMID, NULL) == -1)  errExit("msgctl");
}

int main(int argc, char *argv[]){
    
    struct requestMsg req;
    struct responseMsg res;

    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s pathname\n", argv[0]);
    }

    if(strlen(argv[1]) > sizeof(req.pathname) - 1){
        cmdLineErr("pathname too long (max: %ld bytes)\n", (long)sizeof(req.pathname) - 1);
    }

    int serverId = msgget(SERVER_KEY, S_IWUSR);
    printf("serverId : %d\n", serverId);
    if(serverId == -1)  errExit("msgget - server message queue");
    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    printf("clientId : %d\n", clientId);
    if(clientId == -1)  errExit("msgget - client message queue");

    if(atexit(removeQueue) != 0)    errExit("atexit");

    //发送request给server
    req.mtype = 1;  //随便写，这个例子没用到
    req.clientId = clientId;
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);   //老得想着处理结束符号
    req.pathname[sizeof(req.pathname) - 1] = '\0';  //还得自己追加，但凡键盘输入的都不可信

    if(msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1)   errExit("msgsnd");

    printf("client send over...\n");

    //等待接收response
    ssize_t msgLen = msgrcv(clientId, &res, RES_MSG_SIZE, 0, 0);
    printf("received message : %ld bytes\n", (long)msgLen);
    if(msgLen == -1)    errExit("msgrcv");
    if(res.mtype == RES_MT_FAILURE){
        printf("%s\n", res.data);   //打印失败消息
        if(msgctl(clientId, IPC_RMID, NULL) == -1)  errExit("msgctl");
        exit(EXIT_FAILURE);
    }

    ssize_t totalBytes = msgLen;    //第一次得到正式数据
    int numMsgs = 1;
    while(res.mtype == RES_MT_DATA){ //不会再得到FAIL了，要么是DATA要么是END，也就是说numMsgs至少为2
        msgLen = msgrcv(clientId, &res, RES_MSG_SIZE, 0, 0);
        if(msgLen == -1)    errExit("msgrcv");
        totalBytes += msgLen;   //尝试追加，不一定有了
        numMsgs++;
    }

    printf("Received %ld bytes (%d messages)\n", (long)totalBytes, numMsgs);
    exit(EXIT_SUCCESS);
}

   

