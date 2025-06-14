#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include"socket.h"
#include "dprintf_.h"

void dprintf_(int fd,const char* fmt, ...)
{
    char s[1024*64];
    int ret;
    va_list arg;
    va_start(arg, fmt);
    vsprintf(s, fmt, arg);
    va_end(arg);
    ret=send(fd, s, strlen(s), 0);
}


