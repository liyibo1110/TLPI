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

    //获取传入的文件名
    char *filename = strrchr(argv[1], '/');
    if(filename != NULL){  //如果找到了，说明给的是带目录名的，需要去掉最后的/字符
        filename++;
    }else{  //没找到则说明给的只有basename本身，不需要额外处理
        filename = argv[1];
    }
    
    execle(argv[1], filename, "hello world", (char *)NULL, envVec);
    errExit("execle");  //到这步，说明上面出错了
}