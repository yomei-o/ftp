#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "client_commands.h"
#include "connect_to_server.h"
#include "socket.h"

#define MAX_BUF 4096

#include "mutex_.h"
#include "BS_thread_pool.h"
#include "usleep_.h"
#include "nicaddr.h"

#if defined(_WIN32)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define realpath(a,b) _fullpath(b,a,1024)
#endif

char lastCommand[MAX_BUF];
char lastRecv[MAX_BUF];

struct cmd_chg_list {
    const char* cmd;
    const char* ftp;
};

struct cmd_chg_list cmd_chg_list[] = {
    {"ls","LIST"},
    {"cd","CWD"},
    {"pwd","PWD"},
    {"quit","QUIT"},
    {"put","STOR"},
    {"get","RETR"},
    {NULL,NULL}
};
void changecmd(char* ret, const char* cmd)
{
    int i;
    for (i = 0; i < 100; i++) {
        if (cmd_chg_list[i].cmd == NULL)break;
        if (strncmp(cmd, cmd_chg_list[i].cmd, strlen(cmd_chg_list[i].cmd)) == 0) {
            ret[0] = 0;
            strcpy(ret, cmd_chg_list[i].ftp);
            char* p = cmd;
            while (*p && *p != ' ' && *p != '\r' && *p != '\n')p++;
            strcat(ret, p);
            return;
        }
    }
    strcpy(ret, cmd);
}

static void read_from_server(void* arg)
{
    t_client_info* client_info = (t_client_info*)arg;
    char receive_buf[MAX_BUF];
    char next_buf[MAX_BUF];
    int read_len = 0;
    while ((read_len = recv(client_info->server.fd, receive_buf, MAX_BUF - 1,0))
        > 0) {
        receive_buf[read_len] = 0;
        char* print_pointer = receive_buf;
        char* next_pointer = print_pointer;
        strcpy(lastRecv, receive_buf);
        while (print_pointer) {
            int rb_len = strlen(print_pointer);
            while (*(next_pointer) != 0 && !( *(next_pointer) == '\r' && *(next_pointer + 1) == '\n'))
                ++next_pointer;
            next_pointer += 2;
            if (next_pointer - print_pointer >= rb_len)
                next_pointer = NULL;
            if (next_pointer != NULL) {
                sprintf(next_buf, "%s", next_pointer);
                *next_pointer = 0;
                next_pointer = next_buf;
            }
            if (strncasecmp(
                    lastCommand, S_RESPONSE_PASS, strlen(S_RESPONSE_PASS))
                == 0) {
                client_command_pass(client_info, print_pointer);
            } else if (strncasecmp(lastCommand, S_RESPONSE_PASV,
                           strlen(S_RESPONSE_PASV))
                == 0) {
                client_command_pasv(client_info, print_pointer);
            } else if (strncasecmp(lastCommand, S_RESPONSE_PORT,
                           strlen(S_RESPONSE_PORT))
                == 0) {
                client_command_port(client_info, lastCommand);
            } else if (strncasecmp(print_pointer, S_RESPONSE_CWD,
                           strlen(S_RESPONSE_CWD))
                == 0) {
                client_command_cwd(client_info, print_pointer);
            } else if (strncasecmp(lastCommand, S_RESPONSE_CDUP,
                           strlen(S_RESPONSE_CDUP))
                == 0) {
                client_command_cdup(client_info, print_pointer);
            }
            rb_len = strlen(print_pointer);
            pthread_mutex_lock_print_mutex();
            printf("%s", print_pointer);
            pthread_mutex_unlock_print_mutex();
            if (print_pointer[rb_len - 1] == '\n'
                && print_pointer[rb_len - 2] == '\r') {
                pthread_mutex_n_lock_go_on_mutex();
                client_info->go_on = true;
                pthread_cond_signal_cond();
                pthread_mutex_n_unlock_go_on_mutex();
            }
            if (next_pointer != NULL) {
                sprintf(receive_buf, "%s", next_buf);
                next_pointer = receive_buf;
            }
            print_pointer = next_pointer;
        }
    }
    return;
}

int connect_to_server(t_client_info* client_info, char* cwd)
{
    client_info->client.addrlen = sizeof(struct sockaddr_in);
    client_info->mode = ACTIVE;

    if ((connect(client_info->server.fd,
            (struct sockaddr*)&(client_info->server.addr),
            client_info->server.addrlen))
        < 0)
        perror(ECONNECT);
    getsockname(client_info->server.fd,
        (struct sockaddr*)&client_info->client.addr,
        &client_info->client.addrlen);
    data_socket_open(&client_info->dataListen, "127.0.0.1",
        ntohs(client_info->client.addr.sin_port));
    /* data_socket_open(&client_info->dataListen, "127.0.0.1",
     *     ntohs(client_info->client.addr.sin_port)); */
    //pthread_mutex_init_print_mutex();
    //pthread_mutex_init_go_on_mutex();
    //pthread_cond_init(&client_info->cond, NULL);
    memset(lastCommand, 0, sizeof(lastCommand));

    client_info->go_on = false;
    //pthread_t pthread_id;
    //pthread_create(&pthread_id, NULL, read_from_server, (void*)client_info);
    BS_thread_pool_post(read_from_server, (void*)client_info);
    {
        int i;
        for (int i = 0; i < 1000; i++) {
            if (lastRecv[0] != 0)break;
            usleep_(1000 * 10);
        }
    }
    char command_buf[MAX_BUF << 2];
    do {
        int write_len = 0;
        command_buf[0] = 0;
        pthread_mutex_lock_go_on_mutex();
        while (!client_info->go_on) {
            pthread_cond_wait_cond();// (&client_info->cond, &client_info->go_on_mutex);
        }
        if (strncmp(lastRecv, "220", 3) == 0) {
            char tmp[1024] = { 0 };
            char user[1024] = { 0 };
            printf("login: ");
            fgets(tmp, sizeof(tmp), stdin);
            sscanf(tmp, "%s\n", user);
            sprintf(command_buf,"USER %s\r\n",user);
        }
        else if (strncmp(lastRecv, "331", 3) == 0) {
            char tmp[1024] = { 0 };
            char pass[1024] = { 0 };
            printf("password: ");
            fgets(tmp, sizeof(tmp), stdin);
            sscanf(tmp, "%s\n", pass);
            sprintf(command_buf, "PASS %s\r\n", pass);
        }
        else if (strncmp(lastRecv, "230", 3) == 0) {
            int port = 10000;
            unsigned int addr = get_primary_ipv4_be();
            sprintf(command_buf, "PORT %d,%d,%d,%d,%d,%d\r\n", 
                addr & 255, (addr >> 8) & 255, (addr >> 16) & 255, (addr >> 24) & 255,
                port/256,port%256);
        }
        else {
            char tmp[1024] = { 0 };
            printf("ftp> ");
            fgets(tmp, MAX_BUF, stdin);
            changecmd(command_buf, tmp);
        }
        
        if (command_buf[0] == 0) {
            pthread_mutex_unlock_go_on_mutex();
            break;
        }
        if (command_buf[0] > 0 && command_buf[0] <= ' ') {
            pthread_mutex_unlock_go_on_mutex();
            continue;
        }
        if (strstr(command_buf,"\n")==NULL) {
            pthread_mutex_unlock_go_on_mutex();
            continue;
        }
        char* p = strstr(command_buf, "\n");
        if (*(p - 1) != '\r') {
            strcpy(p, "\r\n");
        }

        while ((write_len += send(client_info->server.fd,
            command_buf + write_len, strlen(command_buf) - write_len, 0)),
            write_len < strlen(command_buf));
  
        client_info->go_on = false;
        strcpy(lastCommand, command_buf);
        pthread_mutex_unlock_go_on_mutex();


        if (strncasecmp(command_buf, "list", 4) == 0) {
            if (client_info->is_authenticated == true)
                client_command_list(client_info, command_buf);
        } else if (strncasecmp(command_buf, "retr", 4) == 0) {
            if (client_info->is_authenticated == true)
                client_command_retr(client_info, command_buf + 5);
        } else if (strncasecmp(command_buf, "stor", 4) == 0) {
            if (client_info->is_authenticated == true)
                client_command_stor(client_info, command_buf + 5);
        }else if (strncasecmp(command_buf, "quit", 4) == 0) {
            break;
        }
    } while (1);
    return (0);
}
