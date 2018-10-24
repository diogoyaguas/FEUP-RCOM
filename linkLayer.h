#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "utils.h"

#define FLAG 0x7E
#define TRANSMITTERSA 0x03
#define RECEIVERSA 0x01
#define SETUP 0x03
#define UA 0x07

struct linkLayer {
	char port[20];
	int baudRate;
	unsigned int sequenceNumber;
	unsigned int timeout;
	unsigned int numRetransmissions;
	char SET[5];
	char UAck[5];
  unsigned int frameSLength;
  unsigned int frameILength;
  volatile int retransmit;
};

struct linkLayer ll;

void retransmission(int signum);

int llopen(char * serialport, int status);
int llwrite(unsigned char * buffer, int length);

int establishConnection(int fd, int status);

void sendSet(int fd);
void receiveUA(int fd);

void receiveSet(int fd);
void sendUA(int fd);

void sendI(int fd);
void receiveRR(int fd);

void receiveI(int fd);
void sendRR(int fd);

void sendDisc(int fd); // TO DO
void receiveDisc(int fd); // TO DO
