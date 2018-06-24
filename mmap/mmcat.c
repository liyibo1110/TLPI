#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char *argv[]){
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s file\n", argv[0]);
    }

    int fd =open(argv[1], O_RDONLY);
    if(fd == -1)    errExit("open");

    struct stat sb;
    if(fstat(fd, &sb) == -1)    errExit("fstat");

    void *addr = mmap(NULL,sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(addr == MAP_FAILED)  errExit("mmap");

    if(write(STDOUT_FILENO, addr, sb.st_size) != sb.st_size){
        fatal("partial/failed write");
    }

    exit(EXIT_SUCCESS);
}