#define _GNU_SOURCE //为了使用MSG_EXCEPT标记
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
    fprintf(stderr, "Usage: %s [options] msqid [max-bytes]\n", progName);
    fprintf(stderr, "Permitted options are:\n");
    fprintf(stderr, "   -e          Use MSG_NOERROR flag\n");
    fprintf(stderr, "   -t type     Select message of given type\n");
    fprintf(stderr, "   -n          Use IPC_NOWAIT flag\n");
    #ifdef MSG_EXCEPT
    fprintf(stderr, "   -x          Use MSG_EXCEPT flag\n");
    #endif
    exit(EXIT_FAILURE);
}

extern char *optarg;
extern int optind;

int main(int argc, char *argv[]){
    struct mbuf msg;
    char opt;
    int flags = 0;
    int type = 0;
    while((opt = getopt(argc, argv, "ent:x")) != -1){
        switch(opt){
            case 'e':
                flags |= MSG_NOERROR;
                break;
            case 'n':
                flags |= IPC_NOWAIT;
                break;
            case 't':
                type = atoi(optarg);
                break;
            #ifdef MSG_EXCEPT
            case 'x':
                flags |= MSG_EXCEPT;
                break;
            #endif
            default:
                usageError(argv[0], NULL);
        }  
    }

    //检查参数是否合法（至少要传入msgid参数）
    if(argc < optind + 1 || argc > optind + 2){
        usageError(argv[0], "Wrong number of arguments\n");
    }

    int msqid = getInt(argv[optind], 0, "msqid");
    //检测是否传了msg-text参数
    size_t maxBytes = (argc > optind + 1) ? getInt(argv[optind + 1], 0, "max-bytes") : MAX_MTEXT;

    //读取消息并输出
    int msgLen = msgrcv(msqid, &msg, maxBytes, type, flags);
    if(msgLen == -1)    errExit("msgrcv");
    printf("Received: type=%ld; length=%ld", msg.mtype, (long)msgLen);
    if(msgLen > 0)  printf("; body=%s", msg.mtext);
    printf("\n");
    exit(EXIT_SUCCESS);
}
