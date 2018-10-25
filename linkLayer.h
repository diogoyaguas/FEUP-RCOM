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
#define INPUTS_A 0x03
#define C_DISC 0x0B
#define ESCAPE 0x7D
#define PATTERNFLAG 0x5E
#define PATTERNESCAPE 0x5D
#define CONTROL0 0x00
#define CONTROL1 0X40
#define RR_CONTROL0 0x05 // RR (receiver ready / positive ACK)
#define RR_CONTROL1 0x85
#define REJ_CONTROL0 0x01 // REJ (reject / negative ACK)
#define REJ_CONTROL1 0x81
#define C_SET 0x03

struct linkLayer {
	char port[20];
	int baudRate;
	unsigned int sequenceNumber;
	unsigned int timeout;
	unsigned int numRetransmissions;
	char SET[5];
	char UAck[5];
  size_t frameSLength;
  size_t frameILength;
  volatile int retransmit;
};

struct linkLayer ll;

void retransmission(int signum);

int llopen(char * serialport, int status);
int llwrite(unsigned char * buffer, int length);

int establishConnection(int fd, int status);

void sendSet(int fd);
int byteStuffingMechanism(unsigned char* message, unsigned char* charsRead, int* lengthOfCharsRead);
void receiveUA(int fd);

void receiveSet(int fd);
void sendUA(int fd);

void sendI(int fd);
void receiveRR(int fd);

void receiveI(int fd);
void sendRR(int fd);

int llclose(int fd);
int checkBBC(unsigned char* message, int sizeMessage);
void sendControlMessage(int fd, unsigned char c);

void sendDisc(int fd); // TO DO
void receiveDisc(int fd); // TO DO
