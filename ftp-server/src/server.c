#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "listen_for_client.h"
#include "socket.h"

void ws_init()
{
#ifdef _WIN32
    WSADATA wsaData;
    int err;
    err = WSAStartup(MAKEWORD(2,0), &wsaData);
#endif
}


void ws_done()
{
#ifdef _WIN32
    WSACleanup(); 
#endif
}

int main(int argc, char* argv[])
{
    t_client_info client_buffer;
    char cwd[1024];
    ws_init();
    if (argc != 3)
        return (USAGE(argv[0]), 0);
    if (chdir(argv[2]) == -1)
        return (perror(argv[2]), 1);
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return (perror(NULL), 1);
    memset(&client_buffer, 0, sizeof(t_client_info));
    if (socket_open(&client_buffer.server, atoi(argv[1])) == -1)
        return (1);
    if ((listen_for_client(&client_buffer, cwd)) == -1)
        return (1);
    if (socket_close(client_buffer.server.fd) == -1)
        return (1);
    ws_done();
    return (0);
}
