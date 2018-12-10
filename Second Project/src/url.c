#include "url.h"

int parseURL(const char *str) {
	size_t length = strlen(str);
	char current_substr[256];
	int i = 6; // end index to ftp://
	int j = 0;

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
			if(strcmp(url.host, "") == 0) { // otherwise it will mess up with the possible '/' from the path
				strcpy(url.host, current_substr);
				memset(current_substr, 0, 20);
				j = 0;
				continue;
			}
		}
		if(i == length - 1) { // last character
			current_substr[j] = str[i];
			strcpy(url.path, current_substr);
			break;
		}

		current_substr[j] = str[i];
		j++;
	}

	printf("user: %s\n", url.user);
	printf("password: %s\n", url.password);
	printf("host: %s\n", url.host);
	printf("path: %s\n", url.path);

	return 0;
}

int getIpByHost(const char * host) {

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

	if ((h = gethostbyname(host)) == NULL)
	{
		herror("gethostbyname");
		return -1;
	}

	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));
	strcpy(url.ip, ip);

	printf("IP address: %s\n", url.ip);

	return 0;
}

int getFilename(const char * path) {
	int i, j=0;
	char filename[256];
	size_t length = strlen(path);

	for(i=0; i<length; i++) {

		if(path[i] == '/') {
			memset(filename, 0, 20);
			j=0;
			continue;
		}

		if(i==length-1) { // last iteration
			filename[j] = path[i];
			strcpy(url.filename, filename);
			break;
		}

		filename[j] = path[i];
		j++;
	}

	printf("Filename: %s\n", url.filename);

	return 0;
}
