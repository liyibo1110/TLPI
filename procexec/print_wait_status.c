#include "../lib/error_functions.h"
#include "./print_wait_status.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


void printWaitStatus(const char *msg, int status){
    if(msg != NULL) printf("%s", msg);

    //如果是子进程正常结束
    if(WIFEXITED(status)){
        printf("child exited, status=%d\n", WEXITSTATUS(status));
    }else if(WIFSIGNALED(status)){  //如果被信号杀死
        printf("child killed by signal %d (%s)", WTERMSIG(status), strsignal(WTERMSIG(status)));
        #ifdef WCOREDUMP    //非SUSv3的标准，但大部分实现都会有
            if(WCOREDUMP(status))   printf(" (core dumped)");
        #endif
        printf("\n");
    }else if(WIFSTOPPED(status)){   //如果被信号停止（可以恢复）
        printf("child stopped by signal %d (%s)", WSTOPSIG(status), strsignal(WSTOPSIG(status)));
    }
    #ifdef WIFCONTINUED //不是所有版本都会有
    else if(WIFCONTINUED(status)){
        printf("child continued\n");
    }
    #endif
    else{   //永远都不应该运行到这里
        printf("what happened to this child? (status=%x)\n", (unsigned int)status);
    }

}

