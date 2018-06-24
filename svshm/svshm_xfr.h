#include <sys/stat.h>

#define SHM_KEY 0x1234  //共享内存的key
#define SEM_KEY 0x5678  //信号量的key

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

#define WRITE_SEM 0
#define READ_SEM 1

#define BUF_SIZE 1024

struct shmseg{
    int count;
    char buf[BUF_SIZE];
};