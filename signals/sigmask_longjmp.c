#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include "../lib/error_functions.h"
#include "../lib/signal_functions.h"

static volatile sig_atomic_t canJump = 0;

#ifdef USE_SIGSETJMP
static sigjmp_buf senv;
#else
static jmp_buf env;
#endif

static void handler(int sig){
	//使用了不安全的函数printf，strsignal，printSigMask
	printf("Received signal %d (%s), signal mask is:\n",
			sig, strsignal(sig));
	printSigMask(stdout, NULL);
	if(!canJump){
		printf("'env' buffer not yet set, doing a simple return\n");
		return;
	}

	#ifdef USE_SIGSETJMP
		siglongjmp(senv, 1);
	#else
		longjmp(env, 1);
	#endif
}

int main(void){
	struct sigaction sa;
	printSigMask(stdout, "Signal mask at startup:\n");
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if(sigaction(SIGINT, &sa, NULL) == -1){
		errExit("sigaction");
	}

	#ifdef USE_SIGSETJMP
		printf("Calling sigsetjmp()\n");
		if(sigsetjmp(senv, 1) == 0){
	#else
		printf("Calling setjmp()\n");
		if(setjmp(env) == 0){
	#endif
			canJump = 1;
		}else{
			printSigMask(stdout, "After jump from handler, signal mask is:\n");
		}
	while(true){
		pause();
	}
}


