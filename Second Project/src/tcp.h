#ifndef __TCP_H
#define __TCP_H

/* Struct FTP is responsible for transfer files online */

struct TCP {
    int control_socket;
    int data_socket;
};

struct TCP tcp;

int create_sockets();

#endif /* __TCP_H */
