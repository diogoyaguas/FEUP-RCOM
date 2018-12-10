#ifndef __TCP_H
#define __TCP_H

#include "utils.h"

#define MAX_SIZE 1024

/* Struct FTP is responsible for transfer files online */

struct TCP {
    int control_socket_fd;
    int data_socket_fd;
    char ip[MAX_SIZE];
    int new_port;
};

struct TCP tcp;

/* creates control socket (setting socket_fd) and connects to ftp server */
int connect_to_server(char * ip, int port);
/* writes cmd to the server through socket_fd */
int write_to_server(int socket_fd, char * cmd);
/* reads reply from server, returns 0 if negative, 1 if positive (return value is boolean) */
int read_reply(int socket_fd, char * buf);
int login(char * user, char * password);
int enter_passive_mode();
#endif /* __TCP_H */
