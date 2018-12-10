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

  if(read_reply()) {
    printf("Error reading from socket\n");
    return -1;
  }

  return 0;
}

int login(char * user, char * password) {
  char username_cmd[MAX_SIZE];
  char password_cmd[MAX_SIZE];

  // username
  sprintf(username_cmd, "USER %s\r\n", user);
  printf("%s\n", username_cmd);
  if (write_to_server(username_cmd) < 0) {
    printf("Error writing username command to server\n");
    return -1;
  }

  if (read_reply()) {
    printf("Error reading reply from the server\n");
    return -1;
  }

  // password
  sprintf(password_cmd, "PASS %s\r\n", user);
  printf("%s\n", password_cmd);
  if (write_to_server(password_cmd) < 0) {
    printf("Error writing password command to server\n");
    return -1;
  }

  if (read_reply()) {
    printf("Error reading reply from the server\n");
    return -1;
  }

  return 0;
}

int read_reply() {
  FILE * fp = fdopen(tcp.socket_fd, "r");
  char str[MAX_SIZE];

	do {
		memset(str, 0, MAX_SIZE);
		fgets(str, MAX_SIZE, fp); // reads a line
		printf("%s", str);
	} while (!('1' <= str[0] && str[0] <= '5') || str[3] != ' ');

  return str[0] >= '4';
}

int write_to_server(char * cmd) {
    int ret = write(tcp.socket_fd, cmd, strlen(cmd));
    return ret;
}
