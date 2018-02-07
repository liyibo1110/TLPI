#define _GUN_SOURCE
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "./error_functions.h"

void printSigset(FILE *of, const char *prefix, const sigset_t *sigset){

	int count = 0;
	for(int sig = 1; sig < NSIG; sig++){
		if(sigismember(sigset, sig)){
			count++;
			fprint(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
		}
	}
	if(count == 0){
		fprintf(of, "%s<empty signal set>\n", prefix);
	}
}

/**
 *	打印目前进程的所有信号掩码
 */
int printSigMask(FILE *of, const char *msg){

	if(msg != NULL){
		fprintf(of, "%s", msg);
	}

	sigset_t currMask;
	if(sigpromask(SIG_BLOCK, NULL, &currMask) == -1){
		return -1;
	}

	printSigset(of, "\t\t", &currMask);
	return 0;
}

/**
 * 打印进程目前所有正在等待的信号
 */
int printPendingSigs(FILE *of, const char *msg){

	if(msg != NULL){
		fprintf(of, "%s", msg);
	}

	sigset_t pendingSigs;
	if(sigpending(SIG_BLOCK, NULL, &pendingSigs) == -1){
		return -1;
	}

	printSigset(of, "\t\t", &pendingSigs);
	return 0;
}



