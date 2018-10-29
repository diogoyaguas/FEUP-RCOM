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
#include "applicationLayer.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

int main(int argc, char** argv) {
    struct termios oldtio,newtio;

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

		al.status = TRANSMITTER;
		al.fd = llopen(argv[1], al.status);

		ll.timeout = 3;
    ll.maxRetransmissions = 3;
		ll.numRetransmissions = ll.maxRetransmissions;
		ll.frameSLength = 5;
		ll.retransmit = FALSE;
    ll.sequenceNumber = 0;

    if ( tcgetattr(al.fd,&oldtio) == -1) { /* save current port settings */
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

    tcflush(al.fd, TCIOFLUSH);

    if ( tcsetattr(al.fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n Establishing conection\n");

	//------------------------------------------------

    //estabelecer conexao

		establishConnection(al.fd, al.status);

    tcsetattr(al.fd, TCSANOW, &oldtio);
    close(al.fd);
    return 0;
}
