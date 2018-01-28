#define _GNU_SOURCE
#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
extern char **environ;

int main(int argc, char *argv[]){
    
    char **ep;	
    clearenv();	//清除所有环境变量
    for(int i=1; i<argc; i++){
	if(putenv(argv[i]) != 0){
	    errExit("putenv: %s", argv[i]);
	}
    }

    if(setenv("GREET", "Hello world", 0) == -1){
	errExit("setenv");
    }

    unsetenv("BYE");

    for(ep=environ; *ep!=NULL; ep++){
	puts(*ep);
    }

    exit(EXIT_SUCCESS);
}
