#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/signal_functions.h"
#include "../lib/error_functions.h"
#include "../lib/get_num.h"

static int sigCount[NSIG];	//统计每个信号的次数
static volatile sig_atomic_t gotSigint = 0;	//如果收到SIGINT信号，设置为非0值

static void handler(int sig){
	if(sig == SIGINT){
		gotSigint = 1;
	}else{
		sigCount[sig]++;
	}
}

int main(int argc, char *argv[]){
	printf("%s: PID is %ld\n", argv[0], (long)getpid());
	for(int n=1;n<NSIG;n++){
		signal(n, handler);
	}

	sigset_t pendingMask, blockingMask, emptyMask;
	if(argc > 1){
		int numSecs = getInt(argv[1], GN_GT_0, NULL);
		sigfillset(&blockingMask);	//给填充上所有的信号
		//给进程弄上所有的信号掩码
		if(sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1){
			errExit("sigprocmask");
		}
		printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
		sleep(numSecs);
		//暂停过后，收集所有等待的信号
		if(sigpending(&pendingMask) == -1){
			errExit("sigpending");
		}
		printf("%s: pending signals are: \n", argv[0]);
		printSigset(stdout, "\t\t", &pendingMask);

		sigemptyset(&emptyMask);
		//给进程清空所有的信号掩码
		if(sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1){
			errExit("sigprocmask");
		}
	}

	while(!gotSigint){
		continue;
	}

	//最终输出收到的各信号统计数
	for(int n=1;n<NSIG;n++){
		if(sigCount[n] != 0){
			printf("%s: signal %d caught %d time%s\n", argv[0], n,
					sigCount[n], (sigCount[n] == 1) ? "" : "s");
		}
	}

	exit(EXIT_SUCCESS);
}
