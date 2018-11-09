#include "utils.h"

int getPacketSize() {
	int packetSize = -1;
	while (packetSize <= 0 || packetSize > 1024) {
        system("clear"); //*nix
		printf("<<< What is the maximum packet size? >>>\n");
        printf("[2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]\n\n:::");
		scanf("%d", &packetSize);
        if(IsPowerOfTwo(packetSize) -1) {
            packetSize = -1;
        }
	}

	return packetSize;
}

int IsPowerOfTwo(int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

int getBaudrate() {
  int choice = -1;
  while (getBaudrateNumber(choice) < 0) {
	system("clear"); //*nix
	printf("<<< What is the baudrate value? >>>\n");
	printf("[300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]\n\n::: ");
	scanf("%d", &choice);
  }
	st.c = choice;
  return getBaudrateNumber(choice);
}

int getBaudrateNumber(int choice) {
	switch (choice) {
	case 300:
		strcpy(st.speed, "B300");
		return B300;
	case 600:
		strcpy(st.speed, "B600");
		return B600;
	case 1200:
		strcpy(st.speed, "B1200");
		return B1200;
	case 2400:
		strcpy(st.speed, "B2400");
		return B2400;
	case 4800:
		strcpy(st.speed, "B4800");
		return B4800;
	case 9600:
		strcpy(st.speed, "B9600");
		return B9600;
	case 19200:
		strcpy(st.speed, "B19200");
		return B19200;
	case 38400:
		strcpy(st.speed, "B38400");
		return B38400;
	case 57600:
		strcpy(st.speed, "B57600");
		return B57600;
	case 115200:
		strcpy(st.speed, "B115200");
		return B115200;
	default:
		return -1;
	}
}

void calculateTime() {

	long seconds = finish.tv_sec - start.tv_sec;
  long ns = finish.tv_nsec - start.tv_nsec;

    if (start.tv_nsec > finish.tv_nsec) { // clock underflow
			--seconds;
			ns += 1000000000;
    }
  st.time = (double)seconds + ((double)ns/(double)1000000000); //calulate total time

	return;
}

void printStatistics(int status) {
	printf("\n");
	printf("<<< Statistics >>>\n\n");
	printf("Baud Rate: %s\n", st.speed);
	printf("Packet Size: %d\n", st.packetSize);
	printf("Timeouts: %d\n\n", st.timeout);

	if(status == TRANSMITTER) {
		printf("Sent messages: %d\n", st.msgSent);
		printf("Received RR: %d\n", st.rrRcvd);
		printf("Received REJ: %d\n\n", st.rejRcvd);
	} else {
	printf("Received messages: %d\n", st.msgRcvd);
	printf("Sent RR: %d\n", st.rrSent);
	printf("Sent REJ: %d\n\n", st.rejSent);
}

	printf("Filesize: %d bytes\n", st.filesize);
	printf("Transfer time: %.3f s\n", st.time);
	float r = (st.filesize*8)/(st.time);
	printf("R: %.3f bits/s\n", r);
	float s = (r/st.c);
	printf("S: %.5f \n\n", s);
}
