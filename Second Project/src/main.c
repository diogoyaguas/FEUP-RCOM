#include "url.h"
#include "ftp.h"

int main(int argc, char * argv[]) {

  //ftp://[<user>:<password>@]<host>/<url-path>
  if(argc != 2) {
    printf("Usage: <application executer> ftp://[<user>:<password>@]<host>/<url-path>\nExiting...\n");
    return -1;
  }

  if(parseURL(argv[1] < 0)) {
    printf("Invalid URL\n");
    printf("Usage: <application executer> ftp://[<user>:<password>@]<host>/<url-path>\nExiting... \n");
    return -1;
  }

	return 0;
}
