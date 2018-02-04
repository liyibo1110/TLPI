#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "../lib/error_functions.h"

static void listFiles(const char *dirpath){
    bool isCurrent = strcmp(dirpath, ".") == 0;
    DIR *dirp = opendir(dirpath);
    if(dirp == NULL){
        errMsg("opendir failed on '%s'", dirpath);
        return;
    }

    while(true){
        errno = 0;
        struct dirent *dp = readdir(dirp);
        if(dp == NULL){
            break;
        }
        //忽略当前目录和上一级目录标识
        if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
            continue;
        }
        if(!isCurrent){
            printf("%s/", dirpath);
        }
        printf("%s\n", dp->d_name);
    }
    //检测遍历中，是否因出错而break
    if(errno != 0){
        errExit("readdir");
    }
    if(closedir(dirp) == -1){
        errMsg("closedir");
    }
}

int main(int argc, char *argv[]){
    if(argc > 1 && strcmp(argv[1], "--help") == 0){
        usageErr("%s [dir...]\n", argv[0]);
    }
    if(argc == 1){
        listFiles(".");
    }else{
        for (argv++; *argv == NULL; argv++){
            listFiles(*argv);
        }
    }
    exit(EXIT_SUCCESS);
}