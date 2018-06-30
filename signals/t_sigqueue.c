#include "../lib/error_functions.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){

	if(argc < 4 || strcmp(argv[1], "--help") == 0){
		usageErr("%s pid sig-num data [num-sigs]\n", argv[0]);
	}

    printf("%s: PID is %ld, UID is %ld\n", argv[0], (long)getpid(), (long)getuid());

    long pid = strtol(argv[1], NULL, 10);
    int sig = atoi(argv[2]);
    int data = atoi(argv[3]);
    int numSigs = (argc > 4) ? atoi(argv[4]) : 1;

    union sigval sv;

    //开始发送实时信号
    for(int i = 0; i < numSigs; i++){
        sv.sival_int = data + i;
        if(sigqueue(pid, sig, sv) == -1)    errExit("sigqueue %d", i);
    }
	
	exit(EXIT_SUCCESS);

}


