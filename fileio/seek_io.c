#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    if(argc < 3 || strcmp(argv[1], "--help") == 0){
        usageErr("%s file {r<length>|R<length>|w<string>|s<offset>}...\n", argv[0]);
    }

    int fd = open(argv[1], O_RDWR | O_CREAT, 
		S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|
		S_IROTH|S_IWOTH);
    if(fd == -1){
    	errExit("open");
    }

    char *buf;
    //后面每个参数组尝试运行一次，可以传N个操作
    for(int ap = 2; ap < argc; ap++){
	switch(argv[ap][0]){
	    case 'r':
	    case 'R':{
		size_t len = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
		buf = (char *)malloc(len);
		if(buf == NULL){
		    errExit("malloc");
		}
		ssize_t numRead = read(fd, buf, len);
		if(numRead == -1){
		    errExit("read");
		}
		//检测文件是否已到末尾
		if(numRead == 0){
		    printf("%s: end-of-file\n", argv[ap]);
		}else{
		    printf("%s: ", argv[ap]);
		    //输出每个字节的内容
		    for(int j = 0; j < numRead; j++){
			if(argv[ap][0] == 'r'){ //10进制
			    printf("%c", isprint((unsigned char)buf[j]) ? buf[j] : '?');
			}else{
			    printf("%02x ", (unsigned int)buf[j]);
			}		
		    }
		}
		break;
	    }
	    case 'w':{
		ssize_t numWritten = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
		if(numWritten == -1){
		    errExit("write");
		}
		printf("%s: wrote %ld bytes\n", argv[ap], (long)numWritten);
		break;
	    }
	    case 's':{
		off_t offset = getLong(&argv[ap][1], GN_ANY_BASE ,argv[ap]);
		if(lseek(fd, offset, SEEK_SET) == -1){
		    errExit("lseek");
		}
		printf("%s: seek successed\n", argv[ap]);
		break;
	    }
	    default:{
		cmdLineErr("Argument must start with [rRws]: %s\n", argv[ap]);
	    }   
	}
    }
}
