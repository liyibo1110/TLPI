#include "./rdwrn.h"
#include <unistd.h>
#include <errno.h>

ssize_t readn(int fd, void *buffer, size_t n){
    size_t totalRead = 0;   //无符号类型
    ssize_t numRead;    //有符号类型，所以可能会是负值
    char *buf = buffer; //转成char类型
    while(totalRead < n){   //必须要达到传来的n字节
        numRead = read(fd, buf, n - totalRead);
        if(numRead == 0)    return totalRead;   //说明读到了EOF，就完事了
        if(numRead == -1){
            if(errno == EINTR){ //如果原因是被中断，则继续读取剩下的
                continue;
            }else{
                return -1;
            }
        }
        totalRead += numRead;
        buf += numRead; //缓冲区也得移动，因为只完成了一部分
    }
    return totalRead;   //肯定为n
}

ssize_t writen(int fd, void *buffer, size_t n){
    size_t totalWritten = 0;
    ssize_t numWritten;
    char *buf = buffer;
    while(totalWritten < n){
        numWritten = write(fd, buf, n - totalWritten);
        if(numWritten <= 0){
            if(numWritten == -1 && errno == EINTR){
                continue;
            }else{
                return -1;
            }
        }
        totalWritten += numWritten;
        buf += numWritten;
    }
    return totalWritten;
}