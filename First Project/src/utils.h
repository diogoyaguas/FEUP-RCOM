#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define TRANSMITTER 1
#define RECEIVER 0
#define FALSE 0
#define TRUE 1

struct statistics {
  char speed[20];
  unsigned int timeout;
  unsigned int packetSize;
	unsigned int msgSent;
	unsigned int msgRcvd;
	unsigned int rrSent;
	unsigned int rrRcvd;
  unsigned int rejSent;
  unsigned int rejRcvd;
  int filesize;
  float time;
  int c;
};

struct statistics st;

 struct timespec start, finish;

int getPacketSize();

int IsPowerOfTwo(int x);

int getBaudrate();

int getBaudrateNumber(int choice);

void calculateTime();

void printStatistics();
