#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "client_commands.h"
#include "socket.h"
#include "usleep_.h"

#if defined(_WIN32)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define realpath(a,b) _fullpath(b,a,1024)
#endif


t_command_alias client_commands[35] = { { "USER", &client_command_user },
    { "PASS", &client_command_pass }, { "ACCT", &client_command_unimplemented },
    { "CWD", &client_command_cwd }, { "CDUP", &client_command_cdup },
    { "SMNT", &client_command_unimplemented },
    { "REIN", &client_command_unimplemented }, { "QUIT", &client_command_quit },
    { "PORT", &client_command_port }, { "PASV", &client_command_pasv },
    { "TYPE", &client_command_unimplemented },
    { "STRU", &client_command_unimplemented },
    { "MODE", &client_command_unimplemented }, { "RETR", &client_command_retr },
    { "STOR", &client_command_stor }, { "STOU", &client_command_unimplemented },
    { "APPE", &client_command_unimplemented },
    { "ALLO", &client_command_unimplemented },
    { "REST", &client_command_unimplemented },
    { "RNFR", &client_command_unimplemented },
    { "RNTO", &client_command_unimplemented },
    { "ABOR", &client_command_unimplemented },
    { "DELE", &client_command_unimplemented },
    { "RMD", &client_command_unimplemented },
    { "MDK", &client_command_unimplemented }, { "PWD", &client_command_pwd },
    { "LIST", &client_command_list }, { "NLST", &client_command_unimplemented },
    { "SITE", &client_command_unimplemented },
    { "SYST", &client_command_unimplemented },
    { "STAT", &client_command_unimplemented }, { "HELP", &client_command_help },
    { "?", &client_command_help }, { "NOOP", &client_command_noop }, { 0 } };

int client_command_unimplemented(t_client_info* client_info, char* argument)
{
    printf("Yet to implement\n");
    (void)argument;
    return (0);
}

int client_remote_connection(t_client_info* client_info)
{
    client_info->data.addrlen = sizeof(struct sockaddr);
    if (client_info->mode == PASSIVE) {
        usleep_(5000);
        struct protoent* pe;
        if (!(pe = getprotobyname(PROTOTYPE_NAME))
            || (client_info->data.fd = socket(client_info->data.addr.sin_family,
                    SOCK_STREAM, pe->p_proto))
                == -1)
            return (-1);
        if (connect(client_info->data.fd,
                (struct sockaddr*)&client_info->data.addr,
                client_info->data.addrlen)
            == -1)
            return (-1);
    } else if (client_info->mode == ACTIVE) {
        if ((client_info->data.fd = accept(client_info->dataListen.fd,
                 (struct sockaddr*)&client_info->data.addr,
                 &client_info->data.addrlen))
            == -1) {
            return (-1);
        }
    }
    return (0);
}

char* client_get_real_path(char* path, char* root)
{
    char* real_path;
    char ret_path[4096];

    if (path[0] == '/') {
        sprintf(ret_path, "%s/%s", root, path);
        real_path = realpath(strdup(ret_path), NULL);
    } else if (!(real_path = realpath(path, NULL))) {
        return (NULL);
    }
    return (real_path);
}

int client_command_user(t_client_info* client_info, char* argument)
{
    return (0);
}

static int client_command_list_close(t_client_info* client_info)
{
    if (client_info->mode == DEFAULT)
        return (0);
    if (socket_close(client_info->data.fd) == -1)
        return (-1);
    /* if (socket_close(client_info->data.fd))
     *   return (-1); */
    return (0);
}

int client_command_list(t_client_info* client_info, char* argument)
{
    (void)argument;
    if (client_remote_connection(client_info) == -1) {
        perror("Client to Server Conection Failed");
        return (-1);
    }
    client_info->data_in_use = true;
    int read_len = 0;
    char buf[4096];
    while ((read_len = recv(client_info->data.fd, buf, 4096-1,0)) > 0) {
        buf[read_len] = 0;
        printf("%s", buf);
        int buf_len = strlen(buf);
    }
    if (client_command_list_close(client_info) == -1)
        return (-1);
    client_info->data_in_use = false;
    return (0);
}

int client_command_pass(t_client_info* client_info, char* argument)
{
    if (strncasecmp(argument, "230", 3) == 0) {
        client_info->is_authenticated = true;
    }
    return (0);
}

int client_command_cwd(t_client_info* client_info, char* argument)
{
    return (0);
}

int client_command_cdup(t_client_info* client_info, char* argument)
{
    return (0);
}

int client_command_quit(t_client_info* client_info, char* argument)
{
    return (1);
}

int client_command_port(t_client_info* client_info, char* argument)
{
    int a1 = 0, a2=0, a3 = 0, a4 = 0, p1 = 0, p2 = 0;
    if (!client_info->is_authenticated)
        return (0);
    if (client_info->mode == PASSIVE) {
        client_info->mode = ACTIVE;
    }
    socket_close(client_info->dataListen.fd);
    char* pointer = argument + 5;

    sscanf(pointer, "%d,%d,%d,%d,%d,%d", &a1, &a2, &a3, &a4, &p1, &p2);
    int port = 0;
    port = p1 * 256 + p2;
    data_socket_open(&client_info->dataListen, "127.0.0.1", port);
    return (0);
}

int client_command_pasv(
    t_client_info* client_info, char* argument )
{
    int a1 = 0, a2 = 0, a3 = 0, a4 = 0, p1 = 0, p2 = 0;
    char* pointer = strstr(argument,"(");
    socket_close(client_info->dataListen.fd);
    /* get_socket_addr(&client_info->dataListen, "127.0.0.1", 12002); */
    if (pointer == NULL)return 1;
    pointer++;
    sscanf(pointer, "%d,%d,%d,%d,%d,%d", &a1, &a2, &a3, &a4, &p1, &p2);

    int port = 0;
    char addr[32];
    port = p1 * 256 + p2;
    sprintf(addr, "%d.%d.%d.%d", a1, a2, a3, a4);

    client_info->data.addrlen = sizeof(struct sockaddr_in);
    client_info->data.addr.sin_family = AF_INET;
    client_info->data.addr.sin_addr.s_addr = inet_addr(addr);
    /* printf("%d\n", port); */
    client_info->data.addr.sin_port = htons(port);
    client_info->mode = PASSIVE;

    return (0);
}

int client_command_retr(t_client_info* client_info, char* argument)
{
    (void)argument;
    if (client_remote_connection(client_info) == -1) {
        perror("Client to Server Conection Failed");
        return (-1);
    }
    client_info->data_in_use = true;
    int read_len = 0;
    char buf[4096];
    FILE* fd;
    for (int i = 0; i < strlen(argument); ++i) {
        if (argument[i] == '\r' || argument[i] == '\n') {
            argument[i] = 0;
            break;
        }
    }
    if ((fd = fopen(argument,"wb")) == 0) {
        perror("File Open failed");
    }
    if (fd) {
        while ((read_len = recv(client_info->data.fd, buf, 4096, 0)) > 0) {
            fwrite(buf, 1, read_len, fd);
        }
        fclose(fd);
    }
    if (client_command_list_close(client_info) == -1)
        return (-1);
    client_info->data_in_use = false;
    return (0);
}

int client_command_stor(t_client_info* client_info, char* argument)
{

    (void)argument;
    if (client_remote_connection(client_info) == -1) {
        perror("Client to Server Conection Failed");
        return (-1);
    }
    client_info->data_in_use = true;
    int read_len = 0;
    char buf[4096];
    FILE* fd;
    for (int i = 0; i < strlen(argument); ++i) {
        if (argument[i] == '\r' || argument[i] == '\n') {
            argument[i] = 0;
            break;
        }
    }
    if ((fd = fopen(argument,"rb")) ==NULL) {
        perror("File Open failed");
    }
    if (fd) {
        while ((read_len = fread(buf, 1, 4096, fd)) > 0) {
            send(client_info->data.fd, buf, read_len, 0);
        }
        fclose(fd);
    }
    if (client_command_list_close(client_info) == -1)
        return (-1);
    client_info->data_in_use = false;
    return (0);
}

int client_command_pwd(t_client_info* client_info, char* argument)
{
    return (0);
}

int client_command_help(t_client_info* client_info, char* argument)
{
    return (0);
}

int client_command_noop(t_client_info* client_info, char* argument)
{
    return (0);
}
