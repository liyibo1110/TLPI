#include <limits.h>
#include <stddef.h>
#define SERVER_KEY 0x1aaaaaa1

struct requestMsg{
    long mtype; //没用到
    int clientId;   //和pathname一起当作mtext传递
    char pathname[PATH_MAX];
};

//利用offsetof宏动态计算clientId和pathname这2个字段的总字节数，因为存在字节填充问题，所以并不能简单相加
#define REQ_MSG_SIZE (offsetof(struct requestMsg, pathname) - offsetof(struct requestMsg, clientId) + PATH_MAX)
#define RES_MSG_SIZE 8192

struct responseMsg{
    long mtype;
    char data[RES_MSG_SIZE];
};

//返回响应的3种mtype
#define RES_MT_FAILURE  1
#define RES_MT_DATA 2
#define RES_MT_END  3