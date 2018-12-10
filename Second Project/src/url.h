#ifndef __URL_H
#define __URL_H

#include "utils.h"

/* Struct URL is responsible for the process of the URL passed through the command line */

struct UniformResourceLocator
{
    char user[256];
    char password[256];
    char host[256];
    char ip[256];
    char path[256];
    char filename[256];
    int port;
};

struct UniformResourceLocator url;

/* parses URL and sets the struct's user, password, host and path*/
int parseURL(const char *str);
/* retrieves ip through host's name and sets the struct's ip */
int getIpByHost(const char *host);
/* retrieves the filename through its entire path */
int getFilename(const char * path);

#endif /* __URL_H */
