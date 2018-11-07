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

int getBaudrate() {
  int choice = -1;
  while (getBaudrateNumber(choice) < 0) {
	system("clear"); //*nix
	printf("<<< What is the baudrate value? >>>\n");
	printf("[300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]\n\n::: ");
	scanf("%d", &choice);
  }
  return getBaudrateNumber(choice);
}

int getBaudrateNumber(int choice) {
	switch (choice) {
	case 300:
		return B300;
	case 600:
		return B600;
	case 1200:
		return B1200;
	case 2400:
		return B2400;
	case 4800:
		return B4800;
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	default:
		return -1;
	}
}

int IsPowerOfTwo(int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}
