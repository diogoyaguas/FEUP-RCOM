#include "url.h"

void initURL()
{

	memset(url.user, 0, 256);
	memset(url.password, 0, 256);
	memset(url.host, 0, 256);
	memset(url.path, 0, 256);
	memset(url.filename, 0, 256);
	url.port = 21;
}

int parseURL(const char *str)
{

	int mode;
	char* activeExpression;
	char* tempURL = (char*) malloc(strlen(str));

	memcpy(tempURL, str, strlen(str));

	if (tempURL[6] == '[') {
		mode = 1; //normal
	} else if(tempURL[6] == '<') {
		mode = 0; //anonymous
	} else return -1;

	return 0;
}

int getIpByHost()
{

	struct hostent *h;

	/*
    struct hostent {
	char    *h_name;	    Official name of the host.
    char    **h_aliases;	A NULL-terminated array of alternate names for the host.
	int     h_addrtype;	    The type of address being returned; usually AF_INET.
    int     h_length;	    The length of the address in bytes.
	char    **h_addr_list;	A zero-terminated array of network addresses for the host.
	                        Host addresses are in Network Byte Order.
    */

	if ((h = gethostbyname(url.host)) == NULL)
	{
		herror("gethostbyname");
		return -1;
	}

	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));
	strcpy(url.ip, ip);

	return 0;
}
