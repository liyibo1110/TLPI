#ifndef _SEMUN_H
#define _SEMUN_H

#include <sys/types.h>
#include <sys/sem.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    #if defined(__linux__)
    struct seminfo *__buf;  //linux特有
    #endif
};

#endif