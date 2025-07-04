#ifndef _SOCKET_H
#define _SOCKET_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#if _WIN32
typedef int socklen_t;
#else
#define closesocket(a) close(a)
#endif

#define ERR_INIT_SOCKET "Initialization of socket failed"
#define ERR_INIT_SOCKET_OPT "Configure socket failed"
#define ERR_BIND_OR_LISTEN_SOCKET "Bind or Listen failed"
#define ERR_SOCKET_CLOSE "Socket close failed"

#define MAX_CLIENTS 1024
#define BUF_SIZE 4096

#define PROTOTYPE_NAME "TCP"

typedef enum e_ftp_mode {
    DEFAULT,
    PASSIVE,
    ACTIVE,
} t_ftp_mode;

typedef struct s_socket_info {
    int fd;
    struct sockaddr_in addr;
    socklen_t addrlen;
} t_socket_info;

typedef struct s_client_info {
    t_socket_info client;
    t_socket_info server;
    t_socket_info dataListen;
    t_socket_info data;

    bool data_in_use;

    t_ftp_mode mode;
    char root_dir[BUF_SIZE];
    char cwd[BUF_SIZE];     //current working directory
    char* username;
    bool is_authenticated;
} t_client_info;

int socket_open(t_socket_info* socket_info, unsigned int port);
int socket_close(int socket_fd);
char* socket_get_ip(t_socket_info socket_info);


#endif
