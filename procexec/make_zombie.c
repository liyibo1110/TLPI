#include "../lib/error_functions.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <libgen.h>

#define CMD_SIZE 200

int main(int argc, char *argv[]){

    setbuf(stdout, NULL);
    printf("Parent PID=%ld\n", (long)getpid());
    char cmd[CMD_SIZE];
    pid_t childPid;
    switch(childPid = fork()){
        case -1 :
            errExit("fork");
        case 0 :    //简单打印并立即退出
            printf("Child (PID=%ld) exiting\n", (long)getpid());
            _exit(EXIT_SUCCESS);
        default :
            sleep(3);   //让子线程运行完
            snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
            cmd[CMD_SIZE - 1] = '\0';   //不能忘啊
            system(cmd);    //直接运行查看进程情况

            //向子进程发送kill信号
            if(kill(childPid, SIGKILL) == -1)   errMsg("kill");
            sleep(3);
            printf("After sending SIGKILL to zombie (PID=%ld):\n", (long)childPid);
            system(cmd);    //再查看一次进程情况
            exit(EXIT_SUCCESS);
    }
}
