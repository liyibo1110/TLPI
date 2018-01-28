#define _BSD_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include "../lib/error_functions.h"
#include "ugid_function.h"
int main(void){
    long lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if(lnmax == -1){
        lnmax = 256;
    } 

    char *username = (char *)malloc(lnmax);
    if(username == NULL){
	errExit("malloc");
    }

    //输入用户名值
    printf("Username: ");
    fflush(stdout);
    if(fgets(username, lnmax, stdin) == NULL){
	exit(EXIT_FAILURE);
    }

    size_t len = strlen(username);
    //将带进来的换行符转成结束符
    if(username[len-1] == '\n'){
	username[len-1] = '\0';
    }

    struct passwd *pwd = getpwnam(username);
    if(pwd == NULL){
	fatal("could't get password record");
    }
    struct spwd *spwd = getspnam(username);
    if(spwd == NULL || errno == EACCES){
	fatal("no permission to read shadow password file");
    }

    if(spwd != NULL){
	//立刻用安全的加密密码覆盖
	pwd->pw_passwd = spwd->sp_pwdp;
    }

    //从输入流获取用户输入的密码
    char *password = getpass("Password: ");

    //开始验证
    char *encrypted = crypt(password, pwd->pw_passwd);

    //立刻将password指向的输入内容清理掉
    for(char *p = password; *p != '\0';){
	*p++ = '\0';
    }

    if(encrypted == NULL){
	errExit("crypt");
    }

    bool authOk = strcmp(encrypted, pwd->pw_passwd) == 0;
    if(!authOk){
	printf("Incorrent password\n");
	exit(EXIT_FAILURE);
    }

    printf("Successfully authenticated: UID=%ld\n", (long)pwd->pw_uid);
    exit(EXIT_SUCCESS);
}
