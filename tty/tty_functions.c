#include "../lib/error_functions.h"
#include "./tty_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

int ttySetCbreak(int fd, struct termios *prevTermios){

    struct termios t;
    if(tcgetattr(fd, &t) == -1) return -1;
    //当前的配置暂存到prev中
    if(prevTermios != NULL) *prevTermios = t;

    //开始修改配置
    t.c_lflag &= ~(ICANON | ECHO);  //关闭规范模式以及回显功能
    t.c_lflag |= ISIG;  //开始INTR、QUIT、SUSP信号的有效性
    t.c_iflag &= ~ICRNL;    //关闭CR映射为NL的功能
    t.c_cc[VMIN] = 1;   //输入1个字节就可读
    t.c_cc[VTIME] = 0;  //没有字节永远阻塞
    if(tcsetattr(fd, TCSAFLUSH, &t) == -1)  return -1;
    return 0;
}

int ttySetRaw(int fd, struct termios *prevTermios){

    struct termios t;
    if(tcgetattr(fd, &t) == -1) return -1;
    //当前的配置暂存到prev中
    if(prevTermios != NULL) *prevTermios = t;

    //开始修改配置
    t.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);  //关闭规范模式、信号传递、扩展处理以及回显功能
    t.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR | INPCK | ISTRIP | IXON | PARMRK);    //全关闭了
    t.c_oflag &= ~OPOST;
    t.c_cc[VMIN] = 1;   //输入1个字节就可读
    t.c_cc[VTIME] = 0;  //没有字节永远阻塞
    if(tcsetattr(fd, TCSAFLUSH, &t) == -1)  return -1;
    return 0;
}