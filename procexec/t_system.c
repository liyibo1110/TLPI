#include "../lib/error_functions.h"
#include "./print_wait_status.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_CMD_LEN 200

int main(int argc, char *argv[]){

   char str[MAX_CMD_LEN];
   int status;

    while(true){
        printf("Command: ");
        fflush(stdout);
        //从键盘读取要执行的命令到str
        if(fgets(str, MAX_CMD_LEN, stdin) == NULL)  break;  //为NULL说明认为到了文件尾，同时发送ctrl+D来达成
        status = system(str);
        printf("system() returned: status=0x%04x (%d,%d)\n",
                (unsigned int)status, status >> 8, status & 0xff);

        if(status == -1) {
            errExit("system");
        }else{
            if(WIFEXITED(status) && WEXITSTATUS(status) == 127){
                printf("(Probably) could not invoke shell\n");
            }else{
                printWaitStatus(NULL, status);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
