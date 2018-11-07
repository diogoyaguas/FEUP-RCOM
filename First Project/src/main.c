/*Non-Canonical Input Processing*/
#include "utils.h"
#include "applicationLayer.h"

#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

int main(int argc, char **argv) {

  struct termios oldtio, newtio;

  int baudRate = B38400; // Default Values
  al.fragmentSize = 256;

  if ((argc < 3)) {
    printf("ERROR! This program takes 2 arguments\n");
  }

  if((strcmp("/dev/ttyS0", argv[1]) != 0) && (strcmp("/dev/ttyS1", argv[1]) != 0)) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  if (strcmp("r", argv[2]) == 0) {
    al.status = RECEIVER;
  } else if (strcmp("t", argv[2]) == 0) {
    al.status = TRANSMITTER;
  } else printf("Usage:\tstatus\n\tex: status t");

  al.fragmentSize = getPacketSize();

  baudRate = getBaudrate();

  al.fd = llopen(argv[1], al.status);

  ll.timeout = 3;
  ll.maxRetransmissions = 3;
  ll.numRetransmissions = ll.maxRetransmissions;
  ll.frameSLength = 5;
  ll.retransmit = FALSE;
  ll.sequenceNumber = 0;

  if (tcgetattr(al.fd, &oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = baudRate | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 1; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 0;  /* blocking read until 1 char received */

  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(al.fd, TCIOFLUSH);

  if (tcsetattr(al.fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  if (al.status == RECEIVER) {
    printf("New termios structure set\n");
  } else
    printf("<< New termios structure set >>\nEstablishing conection");

  //------------------------------------------------

  // al do your thing
  go();

  tcsetattr(al.fd, TCSANOW, &oldtio);
  close(al.fd);
  return 0;
}
