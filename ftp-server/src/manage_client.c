#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "manage_client.h"
#include "request.h"
#include "server_command.h"
#include "BS_thread_pool.h"

#ifdef _WIN32
#define SHUT_RDWR 3

#endif
static void manage_new_client_init(t_client_info* client_info, char* cwd)
{
    strcpy(client_info->root_dir, cwd);
    strcpy(client_info->cwd, cwd);
    /* strcat(client_info->cwd, "/"); */
    /* client_info->mode = ACTIVE; */
    REQUEST_RESPONSE(client_info->client.fd, SERVER_220);
    char buf[6][100];
    sprintf(buf[0], "127");
    sprintf(buf[1], "0");
    sprintf(buf[2], "0");
    sprintf(buf[3], "1");
    int port=ntohs(client_info->client.addr.sin_port);
    sprintf(buf[4], "%d", port/256);
    sprintf(buf[5], "%d", port%256);
    char* info[7];
    info[0] = buf[0];
    info[1] = buf[1];
    info[2] = buf[2];
    info[3] = buf[3];
    info[4] = buf[4];
    info[5] = buf[5];
    info[6] = NULL;
    //server_command_port_init_socket(client_info, info);
}

#if 0
int manage_new_client(t_client_info* client_info, char* cwd)
{
    int instance_pid;
    if ((instance_pid = fork()) < 0) {
        perror(ERR_FORK);
        return (-1);
    }
    if (instance_pid != 0) {
        return (0);
    }
    manage_new_client_init(client_info, cwd);
    manage_registered_client(client_info);
    return (1);
}
#endif

static t_client_info s_client_info;
static char s_cwd[BUF_SIZE];

void new_client_loop(void* vp_)
{
    void** vp = vp_;

    t_client_info client_info = s_client_info;
    char cwd[BUF_SIZE];
    strcpy(cwd, s_cwd);

    manage_new_client_init(&client_info, cwd);
    manage_registered_client(&client_info);

}
int manage_new_client(t_client_info* client_info, char* cwd)
{
    s_client_info=*client_info;
    strcpy(s_cwd, cwd);
    BS_thread_pool_post(new_client_loop, NULL);
    return 0;
}


int manage_registered_client(t_client_info* client_info)
{
    int ret = 0;
    char command_buffer[COMMAND_LEN_MAX];

    while ((ret = recv(
                client_info->client.fd, command_buffer, COMMAND_LEN_MAX + 1,0))
        > 0) {
        command_buffer[ret] = 0;
        printf("Received %s", command_buffer);
        if (manage_client_command(client_info, command_buffer) != 0) {
            break;
        }
    }
    if (client_info->username)
        free(client_info->username);
    shutdown(client_info->client.fd, SHUT_RDWR);
    socket_close(client_info->client.fd);
    return (0);
}

int manage_client_command(t_client_info* client_info, char* command)
{
    int i;

    i = 0;
    while (server_commands[i].title) {
        if (strncasecmp(command, server_commands[i].title,
                strlen(server_commands[i].title))
            == 0) {
            return (server_commands[i].exec(
                client_info, command + strlen(server_commands[i].title)));
        }
        i++;
    }
    if (!client_info->is_authenticated) {
        REQUEST_RESPONSE(client_info->client.fd, SERVER_530);
        return (0);
    }
    REQUEST_RESPONSE(client_info->client.fd, SERVER_500);
    return (0);
}
