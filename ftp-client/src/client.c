#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "connect_to_server.h"
#include "socket.h"

void ws_init()
{
#ifdef _WIN32
    WSADATA wsaData;
    int err;
    err = WSAStartup(MAKEWORD(2, 0), &wsaData);
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
    t_client_info client_info;
    char cwd[1024];

    ws_init();
    if (argc != 3)
        return (USAGE(argv[0]), 0);
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return (perror(NULL), 1);
    memset(&client_info, 0, sizeof(t_client_info));
    if (get_socket_addr(&client_info.server, argv[1], atoi(argv[2])) == -1)
        return (1);
    if ((connect_to_server(&client_info, cwd)) == -1)
        return (1);
    if (socket_close(client_info.client.fd) == -1)
        return (1);
    ws_done();
    return (0);
}
