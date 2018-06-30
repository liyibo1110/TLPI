#include "../lib/error_functions.h"
#include "../time/curr_time.h"
#include "./itimerspec_from_str.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

#define TIMER_SIG SIGRTMAX  //自定义信号