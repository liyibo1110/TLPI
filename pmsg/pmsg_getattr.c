#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

int main(int argc, char *argv[]){
    
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s mq-name\n", argv[0]);
    }

    mqd_t mqd = mq_open(argv[1], O_RDONLY);
    if(mqd == (mqd_t)-1)    errExit("mq_open");
    struct mq_attr attr;
    if(mq_getattr(mqd, &attr) == -1)    errExit("mq_getattr");
    printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    printf("Maximum message size:   %ld\n", attr.mq_msgsize);
    printf("# of messages currently on queue:   %ld\n", attr.mq_curmsgs);
    exit(EXIT_SUCCESS);
}
