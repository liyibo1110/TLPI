#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include "../time/curr_time.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/sem.h>

#define MAX_SEMOPS 1000 //上限

static void usageError(const char *progName){
    fprintf(stderr, "Usage: %s semid op[,op...] ...\n\n", progName);
    fprintf(stderr, "'op' is either: <sem#>{+|-}<value>[n][u]\n");
    fprintf(stderr, "            or: <sem#>=0[n]\n");
    fprintf(stderr, "       \"n\" means include IPC_NOWAIT in 'op'\n");
    fprintf(stderr, "       \"u\" means include SEM_UNDO in 'op'\n\n");
    fprintf(stderr, "e.g.: %s 12345 0+1,1-2un\n", progName);
    fprintf(stderr, "      %s 12345 0=0n,1+1,2-1u 1=0\n", progName);
    exit(EXIT_FAILURE);
}

static int parseOps(char *arg, struct sembuf sops[]){
    char *remaining;    //指向arg单个字符的游标，arg是原始参数字符串
    char *sign; //每次解析出来的非法字符
    char *flag; //随后解析出来的额外选项（n或者u）
    char *comma;    //存放逗号的位置
    int numOps;
    for(numOps = 0, remaining = arg; ; numOps++){
        if(numOps >= MAX_SEMOPS){
            cmdLineErr("too many operations\n");
        }
        if(*remaining == '\0'){ //注意是字符比较
            fatal("Trailing comma or empty argument: \"%s\"", arg);
        }
        if(!isdigit((unsigned char)*remaining)){    //检测每组参数，第一个字符必须是数字（标识符）
            cmdLineErr("Expected initial digit: \"%s\"\n", arg);
        }
        sops[numOps].sem_num = strtol(remaining, &sign, 0); //取出标识符，并顺便找到下一个运算符（加号减号或者等号）
        if(*sign == '\0' || strchr("+-=", *sign) == NULL){  //检测sign必须是3种符号之一
            cmdLineErr("Expected '+', '-' or '=' in \"%s\"\n", arg);
        }
        if(!isdigit((unsigned char)*(sign + 1))){   //检测3种符号后，必须还得是数字，代表要参与的信号量值
            cmdLineErr("Expected digit after '%c' in \"%s\"\n", *sign, arg);
        }
        sops[numOps].sem_op = strtol(sign + 1, &flag, 10);
        if(*sign == '-'){
            sops[numOps].sem_op = -sops[numOps].sem_op;
        }else if(*sign == '='){
            if(sops[numOps].sem_op != 0){
                cmdLineErr("Expected \"=0\" in \"%s\"\n", arg);
            }
        }
        sops[numOps].sem_flg = 0;

        //处理后面的n或u选项
        while(true){
            if(*flag == 'n'){
                sops[numOps].sem_flg |= IPC_NOWAIT;
            }else if(*flag == 'u'){
                sops[numOps].sem_flg |= SEM_UNDO;
            }else{
                break;
            }
            *flag++;
        }

        //处理完1组参数，检查后面参数，必须是逗号或者结束符
        if(*flag != ',' && *flag != '\0'){
            cmdLineErr("Bad trailing character (%c) in \"%s\"\n", *flag, arg);
        }

        comma = strchr(remaining, ',');
        if(comma == NULL){
            break;  //没有逗号说明已处理完毕
        }else{
            remaining = comma + 1;  //有逗号则将remaining指向逗号下一个字符
        }
    }
    return numOps + 1;
}

int main(int argc, char *argv[]){
    
    struct sembuf sops[MAX_SEMOPS];
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        usageError(argv[0]);
    }
    for(int index = 2; argv[index] != NULL; index++){
        int nsops = parseOps(argv[index], sops);
        printf("%5ld, %s: about to semop()  [%s]\n", (long)getpid(), currTime("%T"), argv[index]);
        //开始操作op
        if(semop(getInt(argv[1], 0, "semid"), sops, nsops) == -1)   errExit("semop (PID=%ld)", (long)getpid());
        printf("%5ld, %s: semop() completed [%s]\n", (long)getpid(), currTime("%T"), argv[index]);
    }
    exit(EXIT_SUCCESS);
}