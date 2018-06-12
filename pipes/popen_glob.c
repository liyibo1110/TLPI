#include "../lib/error_functions.h"
#include "../procexec/print_wait_status.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

#define POPEN_FMT "/bin/ls -d %s 2> /dev/null"
#define PAT_SIZE 50
#define PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)

int main(int argc, char *argv[]){
    
    char pat[PAT_SIZE];  //用来存文件名pattern
    char popenCmd[PCMD_BUF_SIZE];
    bool badPattern;
    char pathname[PATH_MAX];
    int i, len, fileCnt, status;
    FILE *fp;
    while(true){
        printf("pattern: ");
        fflush(stdout);
        if(fgets(pat,PAT_SIZE, stdin) == NULL)   break;
        len = strlen(pat);
        if(len <= 1){
            continue;
        }
        //将pat弄成标准的C字符串
        if(pat[len - 1] == '\n')    pat[len - 1] = '\0';
    
        //逐个对字符做检查，所有字母和数字以及给定的一些特殊符号可以通过，其他的不行，比如空格
        for(i = 0, badPattern = false; i < len && !badPattern; i++){
            if(!isalnum((unsigned char)pat[i]) && strchr("_*?[^-].", pat[i]) == NULL){
                badPattern = true;
            }
        }

        if(badPattern){
            printf("Bad pattern character: %c\n", pat[i - 1]);
            continue;
        }

        //构建要执行的命令
        snprintf(popenCmd, PCMD_BUF_SIZE, POPEN_FMT, pat);
        popenCmd[PCMD_BUF_SIZE - 1] = '\0';

        fp = popen(popenCmd, "r");
        if(fp == NULL){
            printf("popen() failed\n");
            continue;
        }

        //开始统计结果
        fileCnt = 0;
        while(fgets(pathname, PATH_MAX, fp) != NULL){
            printf("%s", pathname);
            fileCnt++;
        }

        status = pclose(fp);
        printf("    %d matching file%s\n", fileCnt, (fileCnt != 1) ? "s" : "");
        printf("    pclose() status == %#x\n", (unsigned int)status);
        if(status != -1)    printWaitStatus("\t", status);
    }
    exit(EXIT_SUCCESS);
}