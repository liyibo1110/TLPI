#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

static void usageError(const char *progName){
    fprintf(stderr, "Usage: %s [-cx] name size [octal-perms\n", progName);
    fprintf(stderr, "   -c              Create shared memory (O_CREAT)\n");
    fprintf(stderr, "   -x              Create exclusively (O_EXCL)\n");
    exit(EXIT_FAILURE);
}

extern char *optarg;
extern int optind;

int main(int argc, char *argv[]){
    char opt;

    int flags = O_RDWR;
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

    //处理之后，后面还必须要有2个参数
    if(optind + 1 >= argc)  usageError(argv[0]);

    //参数最后还可以追加权限
    //size_t size = getLong(argv[optind + 1], GN_ANY_BASE, "size");
    size_t size = strtol(argv[optind + 1], NULL, 10);
    mode_t perms = (optind + 2 >= argc) ? (S_IRUSR | S_IWUSR) : getInt(argv[optind + 2], GN_BASE_8, "octal-perms");
    
    int fd = shm_open(argv[optind], flags, perms);
    if(fd == -1) errExit("shm_open");
    if(ftruncate(fd, size) == -1)   errExit("ftruncate");

    void *addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)  errExit("mmap");

    exit(EXIT_SUCCESS);
}
