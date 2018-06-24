#include "../lib/error_functions.h"
#include "../svsem/binary_sems.h"
#include "../svsem/semun.h"
#include "./svshm_xfr.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/sem.h>
#include <sys/shm.h>

int main(int argc, char *argv[]){
    
    union semun dummy;

    int semid = semget(SEM_KEY, 0, 0);
    if(semid == -1) errExit("semget");
    
    int shmid = shmget(SHM_KEY, 0, 0);
    if(shmid == -1) errExit("shmget");

    struct shmseg *shmp = shmat(shmid, NULL, SHM_RDONLY);   //只读就可以了
    if(shmp == (void *)-1)  errExit("shmat");

    //开始将数据从共享内存中读取
    int xfrs = 0;
    int bytes = 0;
    while(true){

        //将读信号量置为占用，这里会阻塞，直到对方写入完成
        if(reserveSem(semid, READ_SEM) == -1)  errExit("reserveSem");
        
        //注意访问共享内存是不需要走系统调用的，当写入端改变了内存中的数据，shmseg结构会立即产生相应变化
        if(shmp->count == 0)    break;
        bytes += shmp->count;

        //输出读来的数据
        if(write(STDOUT_FILENO, shmp->buf, shmp->count) != shmp->count){
            fatal("partial/failed write");
        }
    
        //将写信号量置为空闲
        if(releaseSem(semid, WRITE_SEM) == -1)  errExit("releaseSem");
        xfrs++;
    }

    //还需要将写入信号再次释放，不然会造成最后一次这边还没读完，那边就直接关闭共享内存了
    if(releaseSem(semid, WRITE_SEM) == -1)  errExit("releaseSem");

    printf("Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}