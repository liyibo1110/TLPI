#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

static void usageError(const char *progName){
    fprintf(stderr, "Usage: %s [-n] name msg [prio]\n", progName);
    fprintf(stderr, "   -n          Use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}

extern int optind;
int main(int argc, char *argv[]){
    int opt;
    int flags = O_WRONLY;
    
    while((opt = getopt(argc, argv, "n")) != -1){
        switch(opt){
            case 'n':
                flags |= O_NONBLOCK;
                break;
            default:
                usageError(argv[0]);
        }
    }

    //处理之后，后面还必须要有至少2个参数
    if(argc <= optind + 1)  usageError(argv[0]);

    mqd_t mqd = mq_open(argv[optind + 1], flags);
    if(mqd == (mqd_t)-1) errExit("mq_open");
    
    //判断是否还有最后的prio参数
    unsigned int prio = (argc > optind + 2) ? atoi(argv[optind + 2]) : 0;
    if(mq_send(mqd, argv[optind + 1], strlen(argv[optind + 1]), prio) == -1)    errExit("mq_send");
    exit(EXIT_SUCCESS);
}
