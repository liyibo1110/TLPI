#include "./read_line.h"
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
ssize_t readLine(int fd, void *buffer, size_t n){
    //检测参数合法性
    if(n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    char *buf = buffer;
    char ch;
    size_t totalRead = 0;
    ssize_t numRead;
    while(true){
        //每次只读1个字符
        numRead = read(fd, &ch, 1);
        if(numRead == -1){
            if(errno == EINTR){
                continue;   //被中断不算
            }else{
                return -1;
            }
        }else if(numRead == 0){ //说明是读入的是EOF
            if(totalRead == 0){
                return 0;
            }else{
                break;  //正常中断
            }
        }else{
            if(totalRead < n - 1){  //如果没有到n-1的位置就正常增加
                totalRead++;
                *buf++ = ch;
            }   
            if(ch == '\n') break;   //如果遇到了换行符，也正常中断
        }
    }
    *buf = '\0';    //将自增后的buf最后一位填充结尾
    return totalRead;
}

