#include "../lib/error_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    //有且仅有1个参数--文件名
    if(argc != 2 || strcmp(argv[1], "--help") == 0){
        usageErr("%s pathname\n", argv[0]);
    }

    //定义3个元素的全局环境变量
    char *envVec[] = {"GREET=salut", "BYE=adieu", NULL};

    char *argVec[10];   //定义足够大的参数列表
    //获取传入的文件名
    argVec[0] = strrchr(argv[1], '/');
    if(argVec[0] != NULL){  //如果找到了，说明给的是带目录名的，需要去掉最后的/字符
        argVec[0]++;
    }else{  //没找到则说明给的只有basename本身，不需要额外处理
        argVec[0] = argv[1];
    }
    //追加3个入参
    argVec[1] = "hello world";
    argVec[2] = "goodbye";
    argVec[3] = NULL;   //最后必须以NULL作为结尾，因为数组未初始化，都会是0

    execve(argv[1], argVec, envVec);
    errExit("execve");  //到这步，说明上面出错了
}