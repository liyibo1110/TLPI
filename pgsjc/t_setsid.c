#include "../lib/error_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){

    if(fork() != 0){
        //除了子进程，别的全直接退出
        _exit(EXIT_SUCCESS);
    }

    //将进程搞成一个新的会话里
    if(setsid() == -1)  errExit("setsid");
    printf("PID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(), (long)getpgrp(), (long)getsid(0));

    if(open("/dev/tty", O_RDWR) == -1)  errExit("open /dev/tty");
    exit(EXIT_SUCCESS);
}