#ifdef USE_MAP_ANON
#define _BSD_SOURCE
#endif

#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MEM_SIZE 10 //只映射前10个字节

int main(int argc, char *argv[]){
   
    int *addr;

    #ifdef USE_MAP_ANON //有特定标记，就可以使用MAP_ANONYMOUS
        printf("USE_MAP_ANON flag not found\n");
        addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if(addr == MAP_FAILED)  errExit("mmap");
    #else   //没有特定标记，就退行使用/dev/zero设备
        printf("found USE_MAP_ANON flag!\n");
        int fd = open("/dev/zero", O_RDWR);
        if(fd == -1)    errExit("open");
        addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(addr == MAP_FAILED)  errExit("mmap");
        if(close(fd) == -1) errExit("close");
    #endif 

    *addr = 1;  //向共享内存写入数据1

    switch(fork()){
        case -1:
            errExit("fork");
        case 0:
            printf("Child started, value = %d\n", *addr);
            (*addr)++;  //在子进程中+1
            //子进程也要munmap自己的部分，因为mmap后是每个进程独占的空间，只是都映射到了同样的物理内存位置
            if(munmap(addr, sizeof(int)) == -1) errExit("munmap");
            exit(EXIT_SUCCESS);
        default:
            //主进程等待子进程完成
            if(wait(NULL) == -1)    errExit("wait");
            printf("In parent, value = %d\n", *addr);
            if(munmap(addr, sizeof(int)) == -1) errExit("munmap");
            exit(EXIT_SUCCESS);
    }

}