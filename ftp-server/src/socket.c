#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#include <windows.h>
#include <winsock.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif


#include "socket.h"
#include "nicaddr.h"

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#ifdef _MSC_VER
#pragma comment(lib,"ws2_32.lib")
#endif

int socket_open(t_socket_info* socket_info, unsigned int port)
{
    struct protoent* protocol;
    int option_value;

    if (!(protocol = getprotobyname(PROTOTYPE_NAME)))
        return (fprintf(stderr, ERR_INIT_SOCKET), -1);
    if ((socket_info->fd = socket(AF_INET, SOCK_STREAM, protocol->p_proto)) < 0)
        return (perror(ERR_INIT_SOCKET), -1);
    option_value = 1;
    if (setsockopt(socket_info->fd, SOL_SOCKET, SO_REUSEADDR, (void*)&option_value,
            sizeof(int))
        == -1)
        perror(ERR_INIT_SOCKET_OPT);
    socket_info->addr.sin_family = AF_INET;
    socket_info->addr.sin_port = htons(port);
    socket_info->addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(socket_info->fd, (const struct sockaddr*)&socket_info->addr,
            sizeof(socket_info->addr))
            == -1
        || listen(socket_info->fd, MAX_CLIENTS) == -1) {
        perror(ERR_BIND_OR_LISTEN_SOCKET);
        return (socket_close(socket_info->fd), -1);
    }
    return (0);
}

char* socket_get_ip(t_socket_info socket_info)
{
    char* str;
    uint32_t addr;

    if ((str = malloc(INET_ADDRSTRLEN)) == NULL)
        return (NULL);
    addr = get_primary_ipv4_be();
    //inet_ntop(AF_INET, &socket_info.addr.sin_addr, str, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);
    return (str);
}

int socket_close(int socket_fd)
{
    if (socket_fd == 0) {
        return 0;
    }
    if (closesocket(socket_fd) == -1) {
        perror(ERR_SOCKET_CLOSE);
        return (-1);
    }

    return (0);
}
