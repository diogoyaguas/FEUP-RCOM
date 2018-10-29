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

#define CONTROLDATA  0x01
#define CONTROLSTART 0x02 // control byte in control packet with value start
#define CONTROLEND   0x03 // control byte in control packet with value end
#define CONTROLT1    0x00 // file's size
#define CONTROLT2    0x01 // file's name

struct applicationLayer {
	int fileDescriptor; // file
	int fd; // serial port
  int status;
	char* filename;
	unsigned char * file_data;
	char controlPacket[];
};

struct applicationLayer al;

int sendData(char *filename);

int receiveData(char *filename);

void sendControlPacket(char* filename, unsigned char control_byte);

int sendPacket(int seqNumber, char * buffer, int length);

void receiveControlPacket();

int receivePacket(unsigned char ** buffer, int seqNumber);
