#include "linkLayer.h"

void retransmission(int signum){
	ll.retransmit = TRUE;
}

int llopen(char * serialport, int status) {
	int fd;
	fd = open(serialport, O_RDWR | O_NOCTTY );

	if (fd < 0) {
		perror("Error opening serial port");
		exit(-1);
	}

	return fd;
}

int establishConnection(int fd, int status) {

	if(status==TRANSMITTER) {
		sendSet(fd);
		receiveUA(fd);
	}
	else if(status==RECEIVER) {
		receiveSet(fd);
		sendUA(fd);
	}

	return 0;
}

void sendSet(int fd) {
  int res;
  ll.SET[0] = FLAG;
  ll.SET[4] = FLAG;
  ll.SET[1] = TRANSMITTERSA;
  ll.SET[2] = SETUP;
  ll.SET[3] = ll.SET[1] ^ ll.SET[2];

  //mandar trama de supervisão
  res = write(fd, ll.SET, ll.frameSLength);

	if(res<0) {
		perror("Writing SET frame error");
		exit(-1);
	}

  printf("SET sent (bytes: %d)\n", res);

  sleep(1);

  signal(SIGALRM, retransmission);
  alarm(ll.timeout);
}

void receiveUA(int fd) {
  enum receiveState {INIT, F, FA, FAC, FACBCC, FACBCCF} receiveState;
  receiveState = INIT;
  int res;
  unsigned char byte;
  int unreceived = TRUE;

  while(unreceived) {
    if(ll.retransmit) {
      if(ll.numRetransmissions == 0) {
        printf("No more retransmissions, leaving.\n");
        exit(-1);
      }
      res = write(fd, ll.SET, ll.frameSLength);
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
        if(byte==RECEIVERSA)
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
        if(byte== (RECEIVERSA^UA))
          receiveState=FACBCC;
        else if(byte==FLAG)
          receiveState=F;
        else {
          receiveState = INIT;
					printf("BCC error while reading UA\n");
        }
        break;
      case FACBCC:
        if(byte==FLAG) {
          receiveState=FACBCCF;
          unreceived = FALSE;
          alarm(0);
          ll.numRetransmissions = 3;
					printf("Received UA\n");
        }
        else receiveState=INIT;
        break;
      default: break;
    }
  }
}

void sendUA(int fd) {
	int res;

	//mandar trama de supervisão
	ll.UAck[0] = FLAG;
	ll.UAck[4] = FLAG;
	ll.UAck[1] = RECEIVERSA;
	ll.UAck[2] = UA;
	ll.UAck[3] = ll.UAck[1] ^ ll.UAck[2];

	res = write(fd, ll.UAck, ll.frameSLength);
	printf("UA sent (bytes: %d)\n", res);
	sleep(1);

	signal(SIGALRM, retransmission);
	alarm(ll.timeout);
}

void receiveSet(int fd) {
	enum receiveState {INIT, F, FA, FAC, FACBCC, FACBCCF} receiveState;
  receiveState = INIT;
  int res;
  unsigned char byte;
  int unreceived = TRUE;

  while(unreceived) {
    res = read(fd, &byte, 1);

    if(res<0){
    	perror("Receiving SET reading error");
    }

    switch(receiveState) {
      case INIT:
        if (byte==FLAG)
          receiveState = F;
        break;
      case F:
        if(byte==TRANSMITTERSA)
          receiveState = FA;
        else if(byte==FLAG)
          receiveState = F;
        else receiveState = INIT;
        break;
      case FA:
        if(byte==SETUP)
          receiveState=FAC;
        else if(byte==FLAG)
          receiveState=F;
        else receiveState = INIT;
        break;
      case FAC:
        if(byte == (TRANSMITTERSA^SETUP))
          receiveState=FACBCC;
        else if(byte==FLAG)
          receiveState=F;
        else {
					printf("BCC error while reading SET\n");
          receiveState = INIT;
        }
        break;
      case FACBCC:
        if(byte==FLAG) {
          receiveState=FACBCCF;
          unreceived = FALSE;
					printf("Received SET\n");
        }
        else receiveState=INIT;
        break;
      default: break;
    }
  }
}
