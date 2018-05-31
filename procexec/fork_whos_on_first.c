#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [num-children]\n", argv[0]);
    }
    
    int numChildren = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-children") : 1;

    setbuf(stdout, NULL);
    pid_t childPid;
    
    //printf("numChildren=%d\n", numChildren);

    for(int j = 0 ; j < numChildren ; j++){
        //printf("enter loop\n");
        switch(childPid = fork()){
            case -1 :
                errExit("fork");
            case 0 :
                printf("%d child\n",j);
                _exit(EXIT_SUCCESS);
            default : 
                printf("%d parent\n",j);    //永远都是parent先执行，除非赶上时间片
                wait(NULL); //等待子进程完事
                break;
        }
    }

    exit(EXIT_SUCCESS);
}
