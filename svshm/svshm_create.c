#include "../lib/error_functions.h"
#include "../lib/get_num.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>

static void usageError(const char *progName, const char *msg){
    if(msg != NULL) fprintf(stderr, "%s", msg);
    fprintf(stderr, "Usage: %s [-cx] {-f pathname | -k key | -p} seg-size [octal-perms]\n", progName);
    fprintf(stderr, "   -c              Use IPC_CREAT flag\n");
    fprintf(stderr, "   -x              Use IPC_EXCL flag\n");
    fprintf(stderr, "   -f pathname     Generate key using ftok()\n");
    fprintf(stderr, "   -k key          Use 'key' as key\n");
    fprintf(stderr, "   -p              Use IPC_PRIVATE key\n");
    exit(EXIT_FAILURE);
}

extern char *optarg;
extern int optind;
int main(int argc, char *argv[]){
    int flags = 0;
    int numKeyFlags = 0;
    char opt;
    long lkey;
    key_t key;
    while((opt = getopt(argc, argv, "cf:k:px")) != -1){
        switch(opt){
            case 'c':
                flags |= IPC_CREAT;
                break;
            case 'p':
                key = IPC_PRIVATE;
                numKeyFlags++;
                break;
            case 'x':
                flags |= IPC_EXCL;
                break;
            case 'f':
                key = ftok(optarg, 1);
                if(key == -1)   errExit("ftok");
                numKeyFlags++;
                break;
            case 'k':
                if(sscanf(optarg, "%li", &lkey) != -1){
                    cmdLineErr("-k option requires a numeric argument\n");
                }
                key = lkey;
                numKeyFlags++;
                break;
            default:
                usageError(argv[0], "Bad option\n");
        }
    }
    //-p,-f,-k只能接受1种参数
    if(numKeyFlags != 1){
        usageError(argv[0], "Exactly one of the options -f, -k, or -p must be supplied\n");
    }

    //printf("optind=%d, argc=%d", optind, argc);

    int segSize = getLong(argv[optind], 0, "seg-size");

    //参数最后还可以追加权限
    unsigned int perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR) : getInt(argv[optind + 1], GN_BASE_8, "octal-perms");
    int shmid = shmget(key, segSize, flags | perms);
    if(shmid == -1) errExit("shmget");
    printf("%d\n", shmid);
    exit(EXIT_SUCCESS);
}
