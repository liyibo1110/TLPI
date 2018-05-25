#include "get_num.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

extern errno;

static void gnFail(const char *fname, const char *msg,
		const char *arg, const char *name){
    fprintf(stderr, "%s error", fname);
    if(name != NULL){
	fprintf(stderr, " (in %s", name);
    }
    fprintf(stderr, ": %s\n", msg);
    if(arg != NULL && *arg != '\0'){
	fprintf(stderr, "	offending text: %s\n", arg);
    }
    exit(EXIT_FAILURE);
}

static long getNum(const char *fname, const char *arg,
		int flags, const char *name){
    long result;
    char *endptr;
    int base;

    //必须要有arg参数
    if(arg == NULL || *arg == '\0'){
    	gnFail(fname, "null or empty string", arg, name);
    }
    //根据传来的flags标记，生成最终要转换的进制参数
    if(flags & GN_ANY_BASE){
    	base = 0;
    }else if(flags & GN_BASE_8){
    	base = 8;
    }else if(flags & GN_BASE_16){
    	base = 16;
    }else{
    	base = 10;
    }

    errno = 0; //先置为0，因为原来很可能不是0

    //尝试转换
    result = strtol(arg, &endptr, base);

    if(errno != 0){
    	gnFail(fname, "strtol() failed", arg, name);
    }
    if(*endptr != '\0'){
    	gnFail(fname, "nonnumberic characters", arg, name);
    }
    if((flags & GN_NONNEG) && result < 0){
    	gnFail(fname, "negative value not allowed", arg, name);
    }
    if((flags & GN_GT_0) && result <= 0){
    	gnFail(fname, "value must be > 0", arg, name);
    }
    return result;
}

long getLong(const char *arg, int flags, const char *name){
    return getNum("getLong", arg, flags, name); 
}

int getInt(const char *arg, int flags, const char *name){
    long result = getNum("getInt", arg, flags, name);
    if(result > INT_MAX || result < INT_MIN){
	gnFail("getInt", "integer out of range", arg, name);
    }
    return (int)result;
}
