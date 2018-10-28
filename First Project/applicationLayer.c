#include "applicationLayer.h"

void sendControlPacket(int fd, char* filename, unsigned char control_byte) {
	struct stat f_information;
	
	if(fstat(fd, &f_information) < 0){
		perror("Couldn't obtain information regarding the file.");
	    exit(-1);
	}

	off_t f_size = f_information.st_size; 		/* total size, in signed bytes */
	unsigned int v1 = sizeof(f_size);
	unsigned int v2 = strlen(filename);


	int startpackage_len = 5 + v1 + v2;			/*In order to assure continuity of 5bytes(C, T1, L1, T2, L2) + correspondent V */
	
	unsigned char startpackage[startpackage_len];

	startpackage[0] = control_byte;
	startpackage[1] = CONTROLT1;
	startpackage[2] = v1;
 	*((off_t *)(startpackage[3])) = f_size; 		/*POSIX standard in which is assigned an integer in regards to a filesize*/
	startpackage[3 + v1] = CONTROLT2;
	startpackage[3 + v1 + 1] = v2; /* +1 from CONTROLT2 */

 	strcat((char *)startpackage + 5 + sizeof(f_information.st_size), filename);


	if(control_byte == CONTROLSTART){
		printf("\n||File: %s ||\n", filename);
		printf("||Size: %ld (bytes)||\n\n", f_size);
	}

	if (!llwrite(fd, startpackage,startpackage_len)) {
		printf("Couldn't write control package.\n");
	    exit(-1);
	}

	return;
}

int sendPacket(int fd, int seqNumber, char *buffer, int length) {

  int totalLength = length + 4;
  unsigned char dataPacket[totalLength];

  dataPacket[0] = CONTROLDATA;
  dataPacket[1] = seqNumber + '0';
  dataPacket[2] = length / 256;
  dataPacket[3] = length % 256;

  memcpy(&dataPacket[4], buffer, length);

  if (llwrite(fd, dataPacket, totalLength) < 0) {

    printf("Error sending data packet");
  }

  return 0;
}

void receiveControlPacket() {}

int receivePacket(int fd, unsigned char **buffer, int seqNumber) {

  unsigned char *information = NULL;
  int K = 0; // number of octets

  if (llread(fd, information) < 0) {

    return -1;
  }

  if (information == NULL)
    return -1;

  int C = information[0] - '0'; // control field
  int N = information[1] - '0'; // sequence number

  if (C != CONTROLDATA) {

    return -1;
  }

  if (N != seqNumber) {

    return -1;
  }

  int L2 = information[2];
  int L1 = information[3];
  K = 256 * L2 + L1;

  memcpy((*buffer), &information[4], K);

  free(information);

  return K;
}
