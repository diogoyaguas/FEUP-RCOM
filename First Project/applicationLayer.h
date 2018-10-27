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

#define CONTROLSTART 0x02 // control byte in control packet with value start
#define CONTROLEND   0x03 // control byte in control packet with value end
#define CONTROLT1    0x00 // file's size
#define CONTROLT2    0x01 // file's name

struct applicationLayer {
	int fileDescriptor;
  	int status;
	char* filename;
	char controlPacket[];
};

struct applicationLayer al;

int initApplicationLayer();

void sendControlPacket();
