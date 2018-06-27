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
    fprintf(stderr, "Usage: %s [-n] name\n", progName);
    fprintf(stderr, "   -n          Use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}

extern int optind;
int main(int argc, char *argv[]){
    int opt;
    int flags = O_RDONLY;
    
    while((opt = getopt(argc, argv, "n")) != -1){
        switch(opt){
            case 'n':
                flags |= O_NONBLOCK;
                break;
            default:
                usageError(argv[0]);
        }
    }

    //处理之后，后面还必须要有至少1个参数
    if(argc <= optind)  usageError(argv[0]);

    mqd_t mqd = mq_open(argv[optind], flags);
    if(mqd == (mqd_t)-1) errExit("mq_open");
    
    struct mq_attr attr;
    if(mq_getattr(mqd, &attr) == -1)    errExit("mq_getattr");
    void *buffer = malloc(attr.mq_msgsize);
    if(buffer == NULL)  errExit("malloc");
    unsigned int prio;
    ssize_t numRead = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    if(numRead == -1)   errExit("mq_receive");

    printf("Read %ld bytes; priority = %u\n", (long)numRead, prio);

    //输出至控制台
    if(write(STDOUT_FILENO, buffer, numRead) == -1) errExit("write");
    write(STDOUT_FILENO, "\n", 1);
    exit(EXIT_SUCCESS);
}
