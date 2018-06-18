#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>

int main(int argc, char *argv[]){
    
    //只能2个参数
    if(argc != 3 && strcmp(argv[1], "--help") == 0){
        usageErr("%s msqid max-bytes\n", argv[0]);
    }

    int msqid = getInt(argv[1], 0, "msqid");
    struct msqid_ds ds;
    if(msgctl(msqid, IPC_STAT, &ds) == -1)  errExit("msgctl");

    //修改副本的值
    ds.msg_qbytes = getInt(argv[2], 0, "max-bytes");

    //更新回去
    if(msgctl(msqid, IPC_SET, &ds) == -1)   errExit("msgctl");

    exit(EXIT_SUCCESS);
}
