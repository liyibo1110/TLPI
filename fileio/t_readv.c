#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>

int main(int argc, char *argv[]){
    
    struct iovec iov[3];
    struct stat myStruct; //buffer1
    int x; //buffer2
    #define STR_SIZE 100
    char str[STR_SIZE];	//buffer3

    if(argc != 2 || strcmp(argv[1], "--help") == 0){
	usageErr("%s file\n", argv[0]);
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
	errExit("open");
    }

    ssize_t totalRequired = 0;

    iov[0].iov_base = &myStruct;
    iov[0].iov_len = sizeof(myStruct);
    totalRequired += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    totalRequired += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    totalRequired += iov[2].iov_len;

    ssize_t numRead = readv(fd, iov, 3);
    if(numRead == -1){
	errExit("readv");
    }

    if(numRead < totalRequired){
	printf("Read fewer bytes than requested\n");
    }

    printf("total bytes requested: %ld; bytes read: %ld\n", (long)totalRequired, (long)numRead);

    exit(EXIT_SUCCESS);
}
