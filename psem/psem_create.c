#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

static void usageError(const char *progName){
    fprintf(stderr, "Usage: %s [-cx] mq-name [octal-perms [value]]\n", progName);
    fprintf(stderr, "   -c              Create semaphore (O_CREAT)\n");
    fprintf(stderr, "   -x              Create exclusively (O_EXCL)\n");
    exit(EXIT_FAILURE);
}

extern char *optarg;
extern int optind;

int main(int argc, char *argv[]){
    char opt;

    int flags = 0;
    while((opt = getopt(argc, argv, "cx")) != -1){
        switch(opt){
            case 'c':
                flags |= O_CREAT;
                break;
            case 'x':
                flags |= O_EXCL;
                break;
            default:
                usageError(argv[0]);
        }
    }

    //处理之后，后面还必须要有参数
    if(optind >= argc)  usageError(argv[0]);

    //参数最后还可以追加权限
    mode_t perms = (optind + 1 >= argc) ? (S_IRUSR | S_IWUSR) : getInt(argv[optind + 1], GN_BASE_8, "octal-perms");
    unsigned int value = (optind + 2 >= argc) ? 0 : getInt(argv[optind + 2], 0, "value");
    sem_t *sem = sem_open(argv[optind], flags, perms, value);
    if(sem == SEM_FAILED) errExit("sem_open");
    exit(EXIT_SUCCESS);
}
