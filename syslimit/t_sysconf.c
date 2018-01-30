#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "../lib/error_functions.h"

extern errno;

static void sysconfPrint(const char *msg, int name){
    long limit;
    errno = 0;
    limit = sysconf(name);
    if(limit != -1){
	printf("%s %ld\n", msg, limit);
    }else{
	if(errno == 0){
	    printf("%s (indeterminate)\n", msg);
	}else{
	    errExit("sysconf %s", msg);
	}
    }
}

int main(void){
    sysconfPrint("_SC_ARG_MAX:", _SC_ARG_MAX);
    sysconfPrint("_SC_LOGIN_NAME__MAX:", _SC_LOGIN_NAME_MAX);
    sysconfPrint("_SC_OPEN_MAX:", _SC_OPEN_MAX);
    sysconfPrint("_SC_NGROUPS_MAX:", _SC_NGROUPS_MAX);
    sysconfPrint("_SC_PAGESIZE:", _SC_PAGESIZE);
    sysconfPrint("_SC_RTSIG_MAX:", _SC_RTSIG_MAX);
    exit(EXIT_SUCCESS);
}
