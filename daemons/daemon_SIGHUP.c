#include "become_daemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>

static const char *LOG_FILE = "/tmp/ds.log";
static const char *CONFIG_FILE = "/tmp/ds.conf";
static FILE *logfp;

static void logMessage(const char *format, ...){
    
    const char *TIMESTAMP_FMT = "%F %X";
    #define TS_BUF_SIZE sizeof("YYYY-MM-DD HH:MM:SS")
    char timestamp[TS_BUF_SIZE];
    time_t t = time(NULL);
    struct tm *loc = localtime(&t);
    if(loc == NULL ||
        strftime(timestamp, TS_BUF_SIZE, TIMESTAMP_FMT, loc) == 0){
            fprintf(logfp, "???Unknown time???: ");
    }else{
        fprintf(logfp, "%s: ", timestamp);
    }
    va_list argList;
    va_start(argList, format);
    vfprintf(logfp, format, argList);
    fprintf(logfp, "\n");
    va_end(argList);
}

static void logOpen(const char *logFilename){
    //m为之前的umask值
    mode_t m = umask(077);
    //只有自己用户可以任意操作
    logfp = fopen(logFilename, "a");
    //改回去
    umask(m);
    if(logfp == NULL)   exit(EXIT_FAILURE);
    //没有缓冲区
    setbuf(logfp, NULL);
    logMessage("Opened log file");  
}

static void logClose(void){
    logMessage(logfp, "Closing log file");
    fclose(logfp);
}

static void readConfigFile(const char *configFilename){
    #define SBUF_SIZE 100
    char str[SBUF_SIZE];
    FILE *configfp = fopen(configFilename, "r");
    if(configfp != NULL){
        if(fgets(str, SBUF_SIZE, configfp) == NULL){
            str[0] = '\0';
        }else{
            str[strlen(str) - 1] = '\0';
        }
        logMessage("Read config file: %s", str);
        fclose(configfp);
    }
}

static volatile sig_atomic_t hupReceived = 0;

static void sighupHandler(int sig){
    hupReceived = 1;
}

int main(int argc, char *argv[]){
    const int SLEEP_TIME = 15;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sighupHandler;
    if(sigaction(SIGHUP, &sa, NULL) == -1)  errExit("sigaction");

    if(becomeDaemon(0) == -1)   errExit("becomeDaemon");

    logOpen(LOG_FILE);
    readConfigFile(CONFIG_FILE);

    int unslept = SLEEP_TIME;
    int count = 0;
    while(true){
        //如果被中断导致sleep未完成，则unslept会大于0
        unslept = sleep(unslept);
        if(hupReceived){    //如果收到了sighup信号，则重新打开log文件，并重读config文件
            logClose();
            logOpen(LOG_FILE);
            readConfigFile(CONFIG_FILE);
            hupReceived = 0;
        }

        if(unslept == 0){
            count++;
            logMessage("Main: %d", count);
            unslept = SLEEP_TIME;   //重新再sleep
        }
    }
}

