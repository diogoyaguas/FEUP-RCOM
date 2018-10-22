#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define TRANSMITTER 1
#define RECEIVER 0
#define CONTROL 0x00
#define FLAG 0x7E
#define A 0x03

struct applicationLayer {
	int fileDescriptor;
	int flag;
};

struct applicationLayer al;

int llopen(int porta, int flag) {
	return open(porta, O_RDWR | O_NOCTTY )
}

int llwrite(int fd, unsigned char * buffer, int length) {
	unsigned int packet_length = length+6;
	unsigned char packet[packet_length];
	unsigned char BCC, previous;

	packet[0] = FLAG;
	packet[1] = A;
	packet[2] = CONTROL;
	packet[3] = A^CONTROL;

	int i;
	previous = buffer[0];
	for(i=4; i<length+4; i++){
		packet[i] = buffer[i-4];
		if(i!=4) {
			BCC = previous^packet[i];
			previous = packet[i];
		}
	}

	packet[packet_length-2] = FLAG;
	packet[packet_length-3] = BCC;

	return write(fd, packet, packet_length);
}
