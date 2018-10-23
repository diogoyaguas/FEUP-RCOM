/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "applicationLayer.c"
#include "linkLayer.c"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

volatile int STOP=FALSE;
volatile int retransmit = FALSE;

int main(int argc, char** argv) {
    struct termios oldtio,newtio;

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS0", argv[1])!=0) &&
          (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS0\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

 	  al.fileDescriptor = llopen(argv[1]);
    if (al.fileDescriptor <0) {
      perror(argv[1]);
      exit(-1);
    }

    if ( tcgetattr(al.fileDescriptor,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 10;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 char received */

  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

    tcflush(al.fileDescriptor, TCIOFLUSH);

    if ( tcsetattr(al.fileDescriptor,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n Establishing conection\n");

	//------------------------------------------------

    //trama	de supervisão SET
    if (sendSet(al.fileDescriptor) < 0) {
      printf("Error while sending SET \n");
      exit(-1);
    }





/*    fgets(buf);

    res = write(fd,buf,strlen(buf) + 1);
    printf("%d bytes written\n", res);

    sleep(2);

    while (STOP==FALSE) {
      res = read(fd,bufread,1);
      bufread[res]=0;
      printf(":%s:%d\n", bufread, res);
      if (bufread[res-1]=='\0') STOP=TRUE;
    }
*/

    if ( tcsetattr(al.fileDescriptor,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(al.fileDescriptor);
    return 0;
}
