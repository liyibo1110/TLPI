#define _BSD_SOURCE //为了使用NI_MAXHOST和NI_MAXSERV常量
#include "./inet_sockets.h"
#include "../lib/error_functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdbool.h>

int inetConnect(const char *host, const char *service, int type){

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;

    //将host和service字面量转成addrinfo格式
    int s = getaddrinfo(host, service, &hints, &result);
    if(s != 0){ //必须为0
        errno = ENOSYS;
        return -1;
    }

    int sfd;
    for(rp = result; rp != NULL; rp = rp->ai_next){
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sfd == -1)   continue;
        //尝试连接
        if(connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) break;
        close(sfd);
    }

    freeaddrinfo(result);
    return rp == NULL ? -1 : sfd;
}

static int inetPassiveSocket(const char *service, int type, socklen_t *addrlen,
                                bool doListen, int backlog){

    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_flags = AI_PASSIVE;    //服务端

    //将service字面量转成addrinfo格式
    int s = getaddrinfo(NULL, service, &hints, &result);
    if(s != 0){ //必须为0
        return -1;
    }

    int sfd;
    int optval = 1;
    for(rp = result; rp != NULL; rp = rp->ai_next){
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sfd == -1)   continue;
        //如果是TCP式服务器，需要设置SO_REUSEADDR位，避免2分钟问题
        if(doListen){
            if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
                close(sfd);
                freeaddrinfo(result);
                return -1;
            }
        }
        //不管TCP还是UDP，都要bind
        if(bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(sfd);
    }

    //如果是TCP，还要listen
    if(rp != NULL && doListen){
        if(listen(sfd, backlog) == -1){
            //例子并没有close(sfd)
            freeaddrinfo(result);
            return -1;
        }
    }

    //如果调用传入了不为NULL的长度变量，则填充使其可用
    if(rp != NULL && addrlen != NULL){
        *addrlen = rp->ai_addrlen;
    }

    freeaddrinfo(result);

    return rp == NULL ? -1 : sfd;
}

int inetListen(const char *service, int backlog, socklen_t *addrlen){
    return inetPassiveSocket(service, SOCK_STREAM, addrlen, true, backlog);
}
int inetBind(const char *service, int type, socklen_t *addrlen){
     return inetPassiveSocket(service, type, addrlen, false, 0);
}

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen,
                        char *addrStr, int addrStrLen){
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if(getnameinfo(addr, addrlen, host, NI_MAXHOST, 
                    service, NI_MAXSERV, NI_NUMERICSERV) == 0){
        snprintf(addrStr, addrStrLen, "(%s, %s)", host, service);
    }else{
        snprintf(addrStr, addrStrLen, "(?UNKNOWN?)");
    }
    addrStr[addrStrLen - 1] = '\0';
    return addrStr;
}