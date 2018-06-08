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
    
    execlp(argv[1], argv[1], "hello world", (char *)NULL);
    errExit("execve");  //到这步，说明上面出错了
}