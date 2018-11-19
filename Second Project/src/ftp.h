#ifndef __FTP_H
#define __FTP_H

/* Struct FTP is responsible for transfer files online */

struct FileTransferProtocol {
    int fdControlSocket;
    int fdDataSocket;
};

struct FileTransferProtocol ftp;

#endif /* __FTP_H */
