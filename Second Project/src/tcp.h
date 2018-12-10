#ifndef __TCP_H
#define __TCP_H

#include "utils.h"

#define PORT 21
#define MAX_SIZE 1024

/* Struct FTP is responsible for transfer files online */

struct TCP {
    int socket_fd;
};

struct TCP tcp;

/* creates control socket (setting socket_fd) and connects to ftp server */
int connect_to_server(char * ip);
/* writes cmd to the server through socket_fd */
int write_to_server(char * cmd);
/* reads reply from server, returns 0 if negative, 1 if positive (return value is boolean) */
int read_reply();
int login(char * user, char * password);
#endif /* __TCP_H */
