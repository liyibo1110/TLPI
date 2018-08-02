#ifndef _RDWRN_H
#define _RDWRN_H

#include <sys/types.h>

ssize_t readn(int fd, void *buf, size_t len);
ssize_t writen(int fd, void *buf, size_t len);

#endif