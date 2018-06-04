#include "../lib/error_functions.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
    char buf[BUF_SIZE];
    //先检查命令格式
    if(argc != 3 || strcmp(argv[1], "--help") == 0){
        usageErr("%s old-file new-file\n", argv[0]);
    }
    int inputFd = open(argv[1], O_RDONLY);
    if(inputFd == -1){
	errExit("opening file %s", argv[1]);
    }
    
    int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR |
	            S_IRGRP | S_IWGRP |
		    S_IROTH | S_IWOTH;	//即rw-rw-rw权限    
    
    int outputFd = open(argv[2], openFlags, filePerms);
    if(outputFd == -1){
	errExit("opening file %s", argv[2]);
    }

    //开始传输文件内容 
    ssize_t numRead;
    while((numRead = read(inputFd, buf, BUF_SIZE)) > 0){
	if(write(outputFd, buf, numRead) != numRead){
	    fatal("couldn't wrtie whole buffer");
        }
    }
    if(numRead == -1){
	errExit("read");
    }
    if(close(inputFd) == -1){
	errExit("close input");
    }
    if(close(outputFd) == -1){
	errExit("close output");
    }

    exit(EXIT_SUCCESS);
}
