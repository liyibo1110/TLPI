#include "become_daemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int becomeDaemon(int flags){

    switch(fork()){
        case -1 : return -1;
        case 0 : break; //子进程继续执行
        default : _exit(EXIT_SUCCESS);  //主进程直接退出
    }

    if(setsid() == -1)  return -1;  //必须要成功开启新会话
    switch(fork()){
        case -1 : return -1;
        case 0 : break; //孙子进程继续执行，这样它不可能是进程组长了
        default : _exit(EXIT_SUCCESS);  //子进程直接退出
    }

    int fd;
    //处理各个标记开关
    if(!(flags & BD_NO_UMASK0))  umask(0);
    if(!(flags & BD_NO_CHDIR))  chdir("/");
    if(!(flags & BD_NO_CLOSE_FILES)){
        int maxfd = sysconf(_SC_OPEN_MAX);
        if(maxfd == -1) maxfd = BD_MAX_CLOSE;
        for(fd = 0; fd < maxfd; fd++){
            close(fd);
        }
    }
    if(!(flags & BD_NO_REOPEN_STD_FDS)){
        close(STDIN_FILENO);   
        fd = open("/dev/null", O_RDWR);
        if(fd != STDIN_FILENO){ //fd应该是0,因为0空出来了
            return -1;
        }
        if(dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO){
            return -1;
        }
        if(dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO){
            return -1;
        }
    }
    return 0;
}

