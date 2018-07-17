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
    
    if(argc != 3 || strcmp(argv[1], "--help") == 0){
        usageErr("%s shm-name string\n", argv[0]);
    }

    int fd = shm_open(argv[1], O_RDWR, 0);
    if(fd == -1)    errExit("shm_open");

    size_t len = strlen(argv[2]);
    if(ftruncate(fd, len) == -1)    errExit("ftruncate");

    printf("Resized to %ld bytes\n", (long)len);

    void *addr = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)  errExit("mmap");

    if(close(fd) == -1) errExit("close");

    printf("copying %ld bytes\n", (long)len);
    //将第2个参数的字符串，移到共享内存中
    memcpy(addr, argv[2], len);
    exit(EXIT_SUCCESS);
}
