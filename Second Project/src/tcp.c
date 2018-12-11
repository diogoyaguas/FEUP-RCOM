#include "tcp.h"

int connect_to_server(char * ip, int port) {
	struct	sockaddr_in server_addr;
  int fd;

	/* server address handling */
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET; /* "sin_family is always set to AF_INET." */
	server_addr.sin_addr.s_addr = inet_addr(ip);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port); /*server TCP port must be network byte ordered */

	/* open a TCP socket */
	if ((fd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    perror("socket()");
    return -1;
  }

  /* connect to the server */
  if(connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
    perror("connect()");
    return -1;
  }

  return fd;
}

int login(char * user, char * password) {
  char username_cmd[MAX_SIZE];
  char password_cmd[MAX_SIZE];
  char buf[MAX_SIZE];

  // username
  sprintf(username_cmd, "USER %s\r\n", user);
  printf("%s\n", username_cmd);
  if (write_to_server(tcp.control_socket_fd, username_cmd) < 0) {
    printf("Error writing USER command\n");
    return -1;
  }

  if (read_reply(tcp.control_socket_fd, buf)) {
    printf("Error reading reply to USER command\n");
    return -1;
  }

  // password
  sprintf(password_cmd, "PASS %s\r\n", password);
  printf("%s\n", password_cmd);
  if (write_to_server(tcp.control_socket_fd, password_cmd) < 0) {
    printf("Error writing PASS command\n");
    return -1;
  }

  if (read_reply(tcp.control_socket_fd, buf)) {
    printf("Error reading reply to PASS command\n");
    return -1;
  }

  return 0;
}

int enter_passive_mode() {
  char buffer[MAX_SIZE];

  // send pasv command
  if (write_to_server(tcp.control_socket_fd, "PASV\r\n") < 0) {
    printf("Error writing PASV command\n");
    return -1;
  }

  if (read_reply(tcp.control_socket_fd, buffer)) {
    printf("Error reading reply to PASV command\n");
    return -1;
  }

  int ip1, ip2, ip3, ip4;
  int port1, port2;

  if ((sscanf(buffer, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &port1, &port2)) < 0) {
    printf("Error entering passive mode\n");
    return -1;
  }

  // gather ip and new port

  sprintf(tcp.new_ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
  tcp.new_port = port1 * 256 + port2;

  printf("IP: %s\n", tcp.new_ip);
  printf("New port: %d\n", tcp.new_port);

  tcp.data_socket_fd = connect_to_server(tcp.new_ip, tcp.new_port);
  if (tcp.data_socket_fd < 0) {
  		printf("Error connecting to the server\n");
  		return -1;
  }

  return 0;
}

int retrieve(char * path) {
  char retr_cmd[MAX_SIZE], buffer[MAX_SIZE];

  sprintf(retr_cmd, "RETR %s\r\n", path);
  printf(">%s",retr_cmd);

  if(write_to_server(tcp.control_socket_fd, retr_cmd) < 0) {
    printf("Error writing RETR command\n");
    return -1;
  }

  if(read_reply(tcp.control_socket_fd, buffer) != 0){
    printf("Error reading reply to RETR command\n");
    return -1;
  }

  return 0;
}

int download(char * filename){
  FILE* dest_file;
  if(!(dest_file = fopen(filename, "w"))) {
		printf("Error opening file %s for writing\n", filename);
		return -1;
	}

  char buffer[MAX_SIZE];
  int res;
  while ((res = read(tcp.data_socket_fd, buffer, MAX_SIZE))) {
    if (res < 0) {
      fprintf(stderr, "Error reading from data socket while downloading\n");
      return -1;
    }

    if ((res = fwrite(buffer, res, 1, dest_file)) < 0) {
      fprintf(stderr, "Error writing to the requested file\n");
      return -1;
    }
  }

	if(read_reply(tcp.control_socket_fd, buffer)) {
		printf("Error reading reply\n");
		return -1;
	}

  fclose(dest_file);
  printf("Finished downloading requested file\n");
  return 0;
}

int disconnect_all() {
	char quit_cmd[MAX_SIZE], buffer[MAX_SIZE];

	// issue command
	sprintf(quit_cmd, "QUIT\r\n");
	if(write_to_server(tcp.control_socket_fd, quit_cmd) < 0) {
		printf("Error writing QUIT command to server\n");
		return -1;
	}

	if(read_reply(tcp.control_socket_fd, buffer)) {
		printf("Error reading reply to QUIT command\n");
		return -1;
	}

	if(close(tcp.control_socket_fd) < 0 || close(tcp.data_socket_fd)){
		printf("Error closing sockets\n");
		return -1;
	}

	printf("\nDisconnected sockets\n");
	return 0;
}

int read_reply(int socket_fd, char * buf) {
  FILE * fp = fdopen(socket_fd, "r");

	do {
		memset(buf, 0, MAX_SIZE);
		fgets(buf, MAX_SIZE, fp); // reads a line
		printf("%s", buf);
	} while (!('1' <= buf[0] && buf[0] <= '5') || buf[3] != ' ');

  return buf[0] >= '4';
}

int write_to_server(int socket_fd, char * cmd) {
    int ret = write(socket_fd, cmd, strlen(cmd));
    return ret;
}
