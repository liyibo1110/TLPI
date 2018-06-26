#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

static void usageError(const char *progName){
    fprintf(stderr, "Usage: %s [-cx] [-m maxmsg] [-s maxsize] mq-name [octal-perms]\n", progName);
    fprintf(stderr, "   -c              Create queue (O_CREAT)\n");
    fprintf(stderr, "   -x              Create exclusively (O_EXCL)\n");
    fprintf(stderr, "   -m maxmsg       Set maximum # of messages\n");
    fprintf(stderr, "   -s maxsize      Set maximum message size\n");
    exit(EXIT_FAILURE);
}

extern char *optarg;
extern int optind;
int main(int argc, char *argv[]){
    char opt;
    struct mq_attr attr;

    //初始化值
    struct mq_attr *attrp = NULL;   //只是指针而已，需要挂实际的结构体
    attr.mq_maxmsg = 50;
    attr.mq_msgsize = 2048;
    int flags = O_RDWR;
    while((opt = getopt(argc, argv, "cxm:s:")) != -1){
        switch(opt){
            case 'c':
                flags |= O_CREAT;
                break;
            case 'x':
                flags |= O_EXCL;
                break;
            case 'm':
                attr.mq_maxmsg = atoi(optarg);
                attrp = &attr;
                break;
            case 's':
                attr.mq_msgsize = atoi(optarg);
                attrp = &attr;
                break;
            default:
                usageError(argv[0]);
        }
    }

    //处理之后，后面还必须要有参数
    if(optind >= argc)  usageError(argv[0]);

    //参数最后还可以追加权限
    mode_t perms = (optind + 1 >= argc) ? (S_IRUSR | S_IWUSR) : getInt(argv[optind + 1], GN_BASE_8, "octal-perms");
    mqd_t mqd = mq_open(argv[optind], flags, perms, attrp);
    if(mqd == (mqd_t)-1) errExit("mq_open");
    exit(EXIT_SUCCESS);
}
