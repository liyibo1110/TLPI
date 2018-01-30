#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "../lib/error_functions.h"

extern errno;

static void fpathconfPrint(const char *msg, int fd, int name){
    long limit;
    errno = 0;
    limit = fpathconf(fd, name);
    if(limit != -1){
	printf("%s %ld\n", msg, limit);
    }else{
	if(errno == 0){
	    printf("%s (indeterminate)\n", msg);
	}else{
	    errExit("fpathconf %s", msg);
	}
    }
}

int main(void){
    fpathconfPrint("_PC_NAME_MAX:", STDIN_FILENO, _PC_NAME_MAX);
    fpathconfPrint("_PC_PATH_MAX:", STDIN_FILENO, _PC_PATH_MAX);
    fpathconfPrint("_PC_PIPE_BUF:", STDIN_FILENO, _PC_PIPE_BUF);
    exit(EXIT_SUCCESS);
}
