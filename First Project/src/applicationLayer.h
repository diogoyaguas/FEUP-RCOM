#include "utils.h"
#include "linkLayer.h"

#define CONTROLDATA  0x01
#define CONTROLSTART 0x02 // control byte in control packet with value start
#define CONTROLEND   0x03 // control byte in control packet with value end
#define CONTROLT1    0x00 // file's size
#define CONTROLT2    0x01 // file's name

struct applicationLayer {
	int fileDescriptor; // file
	int fd; // serial port
	unsigned int fragmentSize;
  int status;
	char * filename;
	unsigned char * file_data;
	off_t fileSize;
	char controlPacket[];
};

struct applicationLayer al;

void go();

int setFile();

int getFile();

int sendData();

int receiveData();

int sendControlPacket(unsigned char control_byte);

int sendPacket(int seqNumber, unsigned char * buffer, int length);

int receiveControlPacket();

int receivePacket(unsigned char ** buffer, int seqNumber);
