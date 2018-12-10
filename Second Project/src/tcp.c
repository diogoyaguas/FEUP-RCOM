#include "tcp.h"

int connect_to_server(char * ip) {
	struct	sockaddr_in server_addr;

	/* server address handling */
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET; /* "sin_family is always set to AF_INET." */
	server_addr.sin_addr.s_addr = inet_addr(ip);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(PORT); /*server TCP port must be network byte ordered */

	/* open a TCP socket */
	if ((tcp.socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    perror("socket()");
    return -1;
  }

  /* connect to the server */
  if(connect(tcp.socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
    perror("connect()");
    return -1;
  }

  read_reply();

  return 0;
}

int read_reply() {
  char buffer[256];

  int res = read(tcp.socket_fd, buffer, 256);

  printf("Reply: %s\n", buffer);
  printf("Bytes lidos: %d\n\n", res);

  return res;
}

int write_to_server(char * cmd) {
    int ret = write(tcp.socket_fd, cmd, strlen(cmd));
    return ret;
}
