#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define CMD_SIZE 200
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
    
    char shellCmd[CMD_SIZE];
    char buf[BUF_SIZE];

    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s temp-file [num-1kB-blocks] \n", argv[0]);
    }

    int numBlocks = (argc > 2) ? 
	             getInt(argv[2], GN_GT_0, "num-1kB-blocks") :
		     100000;

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 
		    S_IRUSR | S_IWUSR);
    if(fd == -1){
	errExit("open");
    }

    if(unlink(argv[1]) == -1){ //将文件移除链接
	errExit("unlink");
    }

    for(int i=0; i<numBlocks; i++){
	if(write(fd, buf, BUF_SIZE) != BUF_SIZE){
	    fatal("partial/failed write");
	}
    }

    snprintf(shellCmd, CMD_SIZE, "df -k `dirname %s`", argv[1]);
    system(shellCmd);

    if(close(fd) == -1){
	errExit("close");
    }

    printf("********** Closed file descriptor\n");

    system(shellCmd);
    exit(EXIT_SUCCESS);
}

