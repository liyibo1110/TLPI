#include "../lib/error_functions.h"
#include "../lib/get_num.h"
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

    int semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if(semid == -1) errExit("semget");
    //给信号0（写入）赋予空闲
    if(initSemAvailable(semid, WRITE_SEM) == -1)    errExit("initSemAvailable");
    //给信号1（读取）赋予占用
    if(initSemInUse(semid, READ_SEM) == -1) errExit("initSemInUse");

    int shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if(shmid == -1) errExit("shmget");

    struct shmseg *shmp = shmat(shmid, NULL, 0);
    if(shmp == (void *)-1)  errExit("shmat");

    //开始将数据从标准输入，复制到共享内存中
    int xfrs = 0;
    int bytes = 0;
    while(true){
        
        //将写信号量置为占用
        if(reserveSem(semid, WRITE_SEM) == -1)  errExit("reserveSem");
        
        //读取数据
        shmp->count = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if(shmp->count == -1)   errExit("read");
        
        //将读信号量置为空闲
        if(releaseSem(semid, READ_SEM) == -1)  errExit("releaseSem");
        //为0说明读取完毕
        if(shmp->count == 0)    break;

        xfrs++;
        bytes += shmp->count;
    }

    //还需要将写入信号再次赋予占用，造成阻塞，等待读取程序主动释放1次写信号，不然直接就都关闭了
    if(reserveSem(semid, WRITE_SEM) == -1)  errExit("reserveSem");

    //到这里才会关闭所有资源
    if(semctl(semid, 0, IPC_RMID, dummy) == -1) errExit("semctl");
    if(shmdt(shmp) == -1) errExit("shmdt");
    if(shmctl(shmid, IPC_RMID, 0) == -1) errExit("shmctl");

    printf("Send %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}