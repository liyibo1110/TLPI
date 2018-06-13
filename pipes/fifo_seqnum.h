#include <sys/types.h>

#define SERVER_FIFO "/tmp/seqnum_sv"    //server端的FIFO
#define CLIENT_FIFO_TEMPLATE    "/tmp/seqnum_cl.%ld"    //client端的FIFO，每个client都对应属于自己的
#define CLIENT_FIFO_NAME_LEN    (sizeof(CLIENT_FIFO_TEMPLATE) + 20)  //20是对应ld参数的最大长度

struct request{
    pid_t pid;  //client的pid
    int seqLen; //想要获取序列值的长度
};

struct response{
    int seqNum; //返回client的序列值
};