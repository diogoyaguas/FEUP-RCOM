#include "applicationLayer.h"

/*
int llwrite(unsigned char * buffer, int length) {
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

	return write(al.fileDescriptor, packet, packet_length);;
}
*/

int initApplicationLayer(char * serialport, int status, char * filename) {

	al = (al*) malloc(sizeof(al));

	al.fileDescriptor = llopen(serialport, status);

	if(al.fileDescriptor < 0){
	    perror("Application Layer - initApplicationLayer");
	    exit(-1);
	}

	al.status = status;

	if(file != NULL) {
		al.fileName = (char *) malloc(sizeof(char) * MAX_FILE_NAME);
		strncpy(al->fileName, filename, MAX_FILE_NAME);
	}
	else return -1;

}

void destroyApplicationLayer() {

	if (al.fileName != NULL)
		free(al.fileName);
	free(al);

	al = NULL;
}


void sendControlPacket() {

}
