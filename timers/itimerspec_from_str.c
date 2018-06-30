#include "./itimerspec_from_str.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * 将字符串形式的时间，转换成itimerspec结构体需要的格式
 * 字符串格式为：value.sec[/value.nsec][:interval.sec[/interval.nsec]]
 * 填充进tsp指针的所属数据，没传的用0填充 
 */
void itimerspecFromStr(char *str, struct itimerspec *tsp){
    //先判断有没有冒号
    char *cptr = strchr(str, ':');
    //如果存在，则将冒号替换成结束符
    if(cptr != NULL){
        *cptr = '\0';
    }

    //再判断有没有斜线，只会判断第一个
    char *sptr = strchr(str, '/');
    if(sptr != NULL){
        *sptr = '\0';
    }

    //这样就可以初始化it_value结构了先
    tsp->it_value.tv_sec = atoi(str);
    tsp->it_value.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;
    //再尝试初始化it_interval
    if(cptr == NULL){
        tsp->it_interval.tv_sec = 0;
        tsp->it_interval.tv_nsec = 0;
    }else{
        sptr = strchr(cptr + 1, '/');
        if(sptr != NULL){
            *sptr = '\0'; 
        }
        tsp->it_interval.tv_sec = atoi(cptr + 1);
        tsp->it_interval.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;
    }
}