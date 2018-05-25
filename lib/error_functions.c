#include "error_functions.h"
#include "ename.c.inc"
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static void terminate(bool useExit3){
    char *s;
    s = getenv("EF_DUMPCODE");
    if(s != NULL && *s != '\0'){
        abort();
    }else if(useExit3){
        exit(EXIT_FAILURE);
    }else{
        _exit(EXIT_FAILURE);
    }
}

static void outputError(bool useErr, int err, bool flushStdout,
		const char *format, va_list ap){
    const int BUF_SIZE = 500;
    char buf[BUF_SIZE];
    char userMsg[BUF_SIZE];
    char errText[BUF_SIZE];    
    //将传来的普通信息，存入userMsg中
    vsnprintf(userMsg, BUF_SIZE, format, ap);

    if(useErr){ //根据传入的useErr参数，决定是否输出详细的错误信息
	snprintf(errText, BUF_SIZE, " [%s %s]", 
		 (err > 0 && err <= MAX_ENAME) ? ename[err] : "?UNKNOWN", strerror(err));
    }else{
        snprintf(errText, BUF_SIZE, ":");	    
    }

    //最终生成输出到buf变量里
    snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);

    if(flushStdout){ //如果开启了刷新开关，则还要强制输出标准输出流
	fflush(stdout);
    }
    fputs(buf, stderr);
    fflush(stderr);
}

void errMsg(const char *format, ...){
    va_list argList;
    int savedErrno = errno;
    va_start(argList, format);
    outputError(true, errno, true, format, argList);
    va_end(argList);
    errno = savedErrno;
}

void errExit(const char *format, ...){
    va_list argList;
    va_start(argList, format);
    outputError(true, errno, true, format, argList);
    va_end(argList);
    terminate(true);
}   

void err_exit(const char *format, ...){
    va_list argList;
    va_start(argList, format);
    outputError(true, errno, false, format, argList);
    va_end(argList);
    terminate(false);
}

void errExitEn(int errnum, const char *format, ...){
    va_list argList;
    va_start(argList, format);
    outputError(true, errnum, true, format, argList);
    va_end(argList);
    terminate(true);
}

void fatal(const char *format, ...){
    va_list argList;
    va_start(argList, format);
    outputError(false, 0, true, format, argList);
    va_end(argList);
    terminate(true);
}

void usageErr(const char *format, ...){
    va_list argList;

    fflush(stdout);

    fprintf(stderr, "Usage: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);
    exit(EXIT_FAILURE);
}

void cmdLineErr(const char *format, ...){
    va_list argList;

    fflush(stdout);

    fprintf(stderr, "Command-line usage error: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);
    exit(EXIT_FAILURE);
}




