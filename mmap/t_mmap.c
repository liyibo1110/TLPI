#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MEM_SIZE 10 //只映射前10个字节

int main(int argc, char *argv[]){
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s file [new-value]\n", argv[0]);
    }

    int fd =open(argv[1], O_RDWR);
    if(fd == -1)    errExit("open");

    void *addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(addr == MAP_FAILED)  errExit("mmap");

    //关闭原始文件，测试联动性
    if(close(fd) == -1) errExit("close");

    //输出addr地址的前10个字符
    printf("Current string=%.*s\n", MEM_SIZE, (char *)addr);

    if(argc > 2){
        if(strlen(argv[2]) >= MEM_SIZE){
            cmdLineErr("'new-value' too large\n");
        }

        memset(addr, 0, MEM_SIZE);
        strncpy(addr, argv[2], MEM_SIZE - 1);

        if(msync(addr, MEM_SIZE, MS_SYNC) == -1)    errExit("msycn");
        printf("Copied \"%s\" to shared memory\n", argv[2]);
    }

    exit(EXIT_SUCCESS);
}