#include "url.h"
#include "ftp.h"

int main(int argc, char *argv[])
{

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

  if (getIpByHost() < 0)
  {
    printf("Cannot find ip to hostname %s.\n", url.host);
    return -1;
  }

  return 0;
}
