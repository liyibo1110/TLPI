#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "../time/curr_time.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
    int pfd[2];
    int dummy;
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s sleep-time...\n", argv[0]);
    }

    setbuf(stdout, NULL);
    printf("%s Parent started\n", currTime("%T"));

    //创建管道
    if(pipe(pfd) == -1) errExit("pipe");

    for(int i = 1; i < argc; i++){
        switch(fork()){
        case -1:
            errExit("fork %d", i);
        case 0:
            //先关闭读入端，只留下输出端
            if(close(pfd[0]) == -1) errExit("close");
            sleep(getInt(argv[i], GN_NONNEG, "sleep-time"));
            printf("%s Child %d (PID=%ld) closing pipe\n",
                    currTime("%T"), i, (long)getpid());
            if(close(pfd[1]) == -1) errExit("close");
            _exit(EXIT_SUCCESS);
        default:
            break;
        }
    }

    
    //printf("turn to Parent...\n");
    if(close(pfd[1]) == -1) errExit("close");
    if(read(pfd[0], &dummy, 1) != 0){
        fatal("parent didn't get EOF");
    }
    printf("%s Parent ready to go\n", currTime("%T"));
    exit(EXIT_SUCCESS);
    
}