#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define FLAG 0x7E
#define SETUP 0x03
#define RECEPTORSA 0x01
#define UA 0x07

#define FALSE 0
#define TRUE 1

struct linkLayer {
	char port[20];
	int baudRate;
	unsigned int sequenceNumber;
	unsigned int timeout;
	unsigned int numRetransmissions;
	char SET[5];
  unsigned int frameLength;
  volatile int retransmit;
  int status;
};

struct linkLayer ll = (struct linkLayer) {.timeout = 3, .numRetransmissions = 3, .retransmit = FALSE, .frameLength = 5};

void retransmission(int signum){
	ll.retransmit = TRUE;
	printf("alarm\n");
}

int sendSet(int fd) {
  int res;
  ll.SET[0] = FLAG;
  ll.SET[4] = FLAG;
  ll.SET[1] = A;
  ll.SET[2] = SETUP;
  ll.SET[3] = ll.SET[1] ^ ll.SET[2];

  //mandar trama de supervisão
  res = write(fd, ll.SET, ll.frameLength);
  printf("SET sent (bytes: %d)\n", res);

  sleep(1);

  signal(SIGALRM, retransmission);
  alarm(ll.timeout);

  return res;
}

int receiveUA(int fd) {
  enum receiveState {INIT, F, FA, FAC, FACBCC, FACBCCF} receiveState;
  receiveState = INIT;
  int res;
  unsigned char byte;
  int unreceived = TRUE;

  while(unreceived) {
    if(ll.retransmit) {
      if(ll.numRetransmissions == 0) {
        printf("No more retransmissions, leaving.\n");
        return -1;
      }
      res = write(fd, ll.SET, ll.frameLength);
      printf("SET sent again (bytes: %d)\n", res);
      receiveState = INIT;
      ll.numRetransmissions--;
      alarm(3);
      ll.retransmit = FALSE;
    }

    res = read(fd, &byte, 1);

    if(res<0){
    perror("Receiving UA reading error");
    }

    switch(receiveState) {
      case INIT:
        if (byte==FLAG)
          receiveState = F;
        break;
      case F:
        if(byte==RECEPTORSA)
          receiveState = FA;
        else if(byte==FLAG)
          receiveState = F;
        else receiveState = INIT;
        break;
      case FA:
        if(byte==UA)
          receiveState=FAC;
        else if(byte==FLAG)
          receiveState=F;
        else receiveState = INIT;
        break;
      case FAC:
        if(byte== (RECEPTORSA^UA))
          receiveState=FACBCC;
        else if(byte==FLAG)
          receiveState=F;
        else {
          receiveState = INIT;
        }
        break;
      case FACBCC:
        if(byte==FLAG) {
          receiveState=FACBCCF;
          unreceived = FALSE;
          alarm(0);
          ll.numRetransmissions = 3;
        }
        else receiveState=INIT;
        break;
      default: break;
    }
  }

  return 0;
}
