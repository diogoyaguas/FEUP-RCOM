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

// S Frames
#define FLAG 0x7E
#define TRANSMITTERSA 0x03
#define RECEIVERSA 0x01
#define SETUP 0x03
#define UA 0x07
#define C_DISC 0x0B
#define RR_CONTROL0 0x05 // RR (receiver ready / positive ACK)
#define RR_CONTROL1 0x85
#define REJ_CONTROL0 0x01 // REJ (reject / negative ACK)
#define REJ_CONTROL1 0x81

#define ESCAPE 0x7D
#define PATTERNFLAG 0x5E
#define PATTERNESCAPE 0x5D
#define CONTROL0 0x00
#define CONTROL1 0X40

struct linkLayer {
	char port[20];
	int baudRate;
	unsigned int sequenceNumber;
	unsigned int timeout;
	unsigned int numRetransmissions;
	unsigned char SET[5];
	unsigned char UAck[5];
	unsigned char DISC[5];
	unsigned char RR[5];
  size_t frameSLength;
  size_t frameILength;
  volatile int retransmit;
};

struct linkLayer ll;

void retransmission(int signum);

int llopen(char * serialport, int status);
int llwrite(unsigned char * buffer, int length);

void setSET();
void setUAck(int status);
void setDisc(int status);
void setRR();
void setRR0();
void setRR1();

int establishConnection(int fd, int status);

void sendSFrame(int fd, unsigned char * frame);
void sendI(int fd);

int byteStuffingMechanism(unsigned char* message, unsigned char* charsRead, int* lengthOfCharsRead);

void receiveUA(int fd, unsigned char * retransmit, unsigned int retransmitSize);
void receiveSet(int fd);
void receiveRR(int fd);
void receiveI(int fd);
void receiveDisc(int fd); // TO DO

int checkBCC(unsigned char* message, int sizeMessage);

void sendControlMessage(int fd, unsigned char c);

int llclose(int fd, int status);
