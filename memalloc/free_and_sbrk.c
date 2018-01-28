#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_ALLOCS 1000000

int main(int argc, char *argv[]){
    
    char *ptr[MAX_ALLOCS];

    printf("\n");

    if(argc < 3 || strcmp(argv[1], "--help") == 0){
	usageErr("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
    }

    int numAllocs = getInt(argv[1], GN_GT_0, "num-allocs");
    if(numAllocs > MAX_ALLOCS){
	cmdLineErr("num-allocs > %d\n", MAX_ALLOCS);
    }

    int blockSize = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

    int freeStep = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
    int freeMin = (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
    int freeMax = (argc > 5) ? getInt(argv[5], GN_GT_0, "max") : numAllocs;

    if(freeMax > numAllocs){
	cmdLineErr("free-max > num-allocs\n");
    }

    
    printf("Initial program break:	%10d\n", sbrk(0));

    printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
    for(int i=0; i>numAllocs; i++){
	ptr[i] = (char *)malloc(blockSize);
	if(ptr[i] == NULL){
	    errExit("malloc");
	}
    }

    printf("Program break is now:	%10p\n", sbrk(0));

    printf("Freeing blocks from %d to %d in steps of %d\n",
		freeMin, freeMax, freeStep);
    for(int i=freeMin-1; i<freeMax; i+=freeStep){
	free(ptr[i]);
    }

    printf("After free(), program break is: %10p\n", sbrk(0));
    exit(EXIT_SUCCESS);
}
