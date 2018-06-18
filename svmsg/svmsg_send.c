#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MTEXT 1024
struct mbuf{
    long mtype;
    char mtext[MAX_MTEXT];
};

static void usageError(const char *progName, const char *msg){
    if(msg != NULL) fprintf(stderr, "%s", msg);
    fprintf(stderr, "Usage: %s [-n] msqid msg-type [msg-text]\n", progName);
    fprintf(stderr, "   -n  -Use IPC_NOWAIT flag\n");
    exit(EXIT_FAILURE);
}

extern char *optarg;
extern int optind;

int main(int argc, char *argv[]){
    struct mbuf msg;
    char opt;
    int flags = 0;
    while((opt = getopt(argc, argv, "n")) != -1){
        if(opt == 'n'){
            flags |= IPC_NOWAIT;
        }else{
            usageError(argv[0], NULL);
        }
    }

    //检查参数是否合法
    if(argc < optind + 2 || argc > optind + 3){
        usageError(argv[0], "Wrong number of arguments\n");
    }

    int msqid = getInt(argv[optind], 0, "msqid");
    msg.mtype = getInt(argv[optind + 1], 0, "msq-type");

    //检测是否传了msg-text参数
    int msgLen;
    if(argc > optind + 2){
        msgLen = strlen(argv[optind + 2]) + 1;
        if(msgLen > MAX_MTEXT){
            cmdLineErr("msg-text too long (max: %d characters)\n", MAX_MTEXT);
        }
        memcpy(msg.mtext, argv[optind + 2], msgLen);
    }else{
        msgLen = 0;
    }

    //发送消息
    if(msgsnd(msqid, &msg, msgLen, flags) == -1)    errExit("msgsnd");
    exit(EXIT_SUCCESS);
}
