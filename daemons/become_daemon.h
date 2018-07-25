#ifndef _BECOME_DAEMON_H
#define _BECOME_DAEMON_H

#define BD_NO_CHDIR 01
#define BD_NO_CLOSE_FILES   02
#define BD_NO_REOPEN_STD_FDS    04
#define BD_NO_UMASK0    010

#define BD_MAX_CLOSE    8192    //循环关闭的最大文件描述符值（只是假定的）

int becomeDaemon(int flags);

#endif