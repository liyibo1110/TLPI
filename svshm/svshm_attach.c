#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>

static void usageError(const char *progName){
    fprintf(stderr, "Usage: %s [shmid:address[rR]]...\n", progName);
    fprintf(stderr, "               r=SHM_RND; R=SHM_RDONLY\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){

    printf("SHMLBA = %ld (%#lx), PID = %ld\n",
            (long)SHMLBA, (unsigned long)SHMLBA, (long)getpid());

    //开始循环处理每组参数
    char *p;    //非法符号
    for(int i = 1; i < argc; i++){
        int shmid = strtol(argv[i], &p, 0);
        if(*p != ':')   usageErr(argv[0]);
        void *addr = (void *)strtol(p + 1, NULL, 0);
        int flags = (strchr(p + 1, 'r') == NULL ? 0 : SHM_RND);
        if(strchr(p + 1, 'R') != NULL)  flags |= SHM_RDONLY;

        void *retAddr = shmat(shmid, addr, flags);
        if(retAddr == (void *)-1)   errExit("shmat: %s", argv[i]);
        printf("%d: %s ==> %p\n", i, argv[i], retAddr);
    }

    printf("Sleeping 5 seconds\n");
    sleep(5);   //为了中途观察

    exit(EXIT_SUCCESS);
}
