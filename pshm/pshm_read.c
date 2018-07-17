#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[]){
    
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s shm-name\n", argv[0]);
    }

    int fd = shm_open(argv[1], O_RDONLY, 0);
    if(fd == -1)    errExit("shm_open");

    struct stat sb;
    if(fstat(fd, &sb) == -1)    errExit("fstat");

    void *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)  errExit("mmap");

    if(close(fd) == -1) errExit("close");

    //向stdout输出内存里面的字符串
    write(STDOUT_FILENO, addr, sb.st_size);
    printf("\n");
    exit(EXIT_SUCCESS);

}
