#include "url.h"

int parseURL(const char *str) {
	size_t length = strlen(str);
	char copy[50];
	char current_substr[20];
	int i = 6; // end index to ftp://
	int j = 0;

	strcpy(copy, str); // whole url
	strncpy(current_substr, str, i); // ftp://

	if(strcmp(current_substr, "ftp://") != 0) {
		printf("URL must start with ftp://\n");
		return -1;
	}

	memset(current_substr, 0, 20); // cleaning it up

	for(i = 6; i<length; i++) {
		if(str[i] == ':') {
			strcpy(url.user, current_substr);
			memset(current_substr, 0, 20);
			j = 0;
			continue;
		}
		if(str[i] == '@') {
			strcpy(url.password, current_substr);
			memset(current_substr, 0, 20);
			j = 0;
			continue;
		}
		if(str[i] == '/'){
			strcpy(url.host, current_substr);
			memset(current_substr, 0, 20);
			j = 0;
			continue;
		}
		if(i == length - 1) { // last character
			current_substr[j] = str[i];
			strcpy(url.path, current_substr);
			break;
		}

		current_substr[j] = str[i];
		j++;
	}
	
	return 0;
}

int getIpByHost() {

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
