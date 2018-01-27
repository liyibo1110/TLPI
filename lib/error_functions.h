#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

#ifdef __GNUC__
  #define NORETURN __attribue__ ((__noreturn__))
#else
  #define NORETURN
#endif

//函数原型
void errMsg(const char *format, ...);
void errExit(const char *format, ...);
void err_exit(const char *format, ...);
void errExitEn(int errnum, const char *format, ...);
void fatal(const char *format, ...);
void usageErr(const char *format, ...);
void cmdLineErr(const char *format, ...);

#endif 
