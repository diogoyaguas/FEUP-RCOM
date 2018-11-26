#ifndef __URL_H
#define __URL_H

#include "utils.h"

/* Struct URL is responsible for the process of the URL passed through the command line */

struct UniformResourceLocator {
    char user[256];
    char password[256];
    char host[256];
    char ip[256];
    char path[256];
    char filename[256];
    int port;
};

struct UniformResourceLocator url;

int parseURL(char * url);
int getIpByHost(struct UniformResourceLocator *url);

#endif /* __URL_H */
