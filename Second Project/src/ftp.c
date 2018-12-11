#include "url.h"
#include "tcp.h"

/*
EXEMPLO PARA TESTAR:
./rcom ftp://anonymous:@ftp.up.pt/debian/README
*/

int main(int argc, char *argv[]) {

  //ftp://[<user>:<password>@]<host>/<url-path>
  if (argc != 2)
  {
    printf("Wrong number of arguments.\n");
    printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\nExiting...\n", argv[0]);
    return -1;
  }

  // start parsing first argument to URL components
  if (parseURL(argv[1]) < 0) {
    printf("Invalid URL.\n");
    printf("Usage: <application executer> ftp://[<user>:<password>@]<host>/<url-path>\nExiting... \n");
    return -1;
  }

  printf("user: %s\n", url.user);
  printf("password: %s\n", url.password);
  printf("host: %s\n", url.host);
  printf("path: %s\n", url.path);
  printf("Filename: %s\n", url.filename);
  printf("IP address: %s\n\n", url.ip);

  // connect to server
  tcp.control_socket_fd = connect_to_server(url.ip, 21);
  if (tcp.control_socket_fd < 0) {
    return -1;
  }
  char buffer[MAX_SIZE];
  if(read_reply(tcp.control_socket_fd, buffer)) {
    printf("Error reading from socket\n");
    return -1;
  }

  // login
  if(login(url.user, url.password) < 0) {
    return -1;
  }

  // go into passive mode
  if(enter_passive_mode() < 0) {
    return -1;
  }

  // retrieve file in one end of the link
  if(retrieve(url.path) < 0){
    return -1;
  }

  // download file in the other end
  if(download(url.filename) < 0) {
    return -1;
  }

  // disconnect sockets
  if(disconnect_all() < 0) {
    return -1;
  }

  printf("Terminating...\n");

  return 0;
}
