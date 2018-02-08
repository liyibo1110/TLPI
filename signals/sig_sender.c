#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include "../lib/error_functions.h"
#include "../lib/get_num.h"
int main(int argc, char *argv[]){

	if(argc < 4 || strcmp(argv[1], "--help") == 0){
		usageErr("%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);
	}

	pid_t pid = getLong(argv[1], 0, "PID");
	int numSigs = getInt(argv[2], GN_GT_0, "num-sigs");
	int sig = getInt(argv[3], 0, "sig-num");

	//开始发送信号
	printf("%s: sending signal %d to process %ld %d times\n",
			argv[0], sig, (long)pid, numSigs);

	for(int i=0; i<numSigs; i++){
		if(kill(pid, sig) == -1){
			errExit("kill");
		}
	}

	//如果有第4个参数，继续发送
	if(argc > 4){
		int sig2 = getInt(argv[4], 0, "sig-num-2");
		if(kill(pid, sig2) == -1){
			errExit(kill);
		}
	}

	printf("%s: exiting\n", argv[0]);
	exit(EXIT_SUCCESS);

}


