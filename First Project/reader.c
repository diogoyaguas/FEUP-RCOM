/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "applicationLayer.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

volatile int STOP = FALSE;

int main(int argc, char** argv)
{
    struct termios oldtio, newtio;

    al.status = RECEIVER;
    al.fd = llopen(argv[1], al.status);
    al.fragmentSize = 256;

    ll.timeout = 3;
    ll.maxRetransmissions = 3;
    ll.numRetransmissions = ll.maxRetransmissions;
    ll.frameSLength = 5;
    ll.retransmit = FALSE;
    ll.sequenceNumber = 0;

    if ((argc < 2) || ((strcmp("/dev/ttyS0", argv[1]) != 0) && (strcmp("/dev/ttyS1", argv[1]) != 0))) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    if (tcgetattr(al.fd, &oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 1; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 0; /* blocking read until 1 char received */

    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

    tcflush(al.fd, TCIOFLUSH);

    if (tcsetattr(al.fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    //------------------------------------------------------------------------------

    //al do your thing
    go();

    tcsetattr(al.fd, TCSANOW, &oldtio);
    close(al.fd);
    return 0;
}
