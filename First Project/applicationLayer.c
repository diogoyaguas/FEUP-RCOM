#include "applicationLayer.h"

void sendControlPacket(unsigned char control_byte) {
	struct stat f_information;
	
	if(fstat(al.fileDescriptor, &f_information) < 0){
		perror("Couldn't obtain status information regarding the file.");
	    exit(-1);
	}

	off_t f_size = f_information.st_size; 		/* total size, in signed bytes */
	unsigned int v1 = sizeof(f_size);
	unsigned int v2 = strlen(al.filename);


	int startpackage_len = 5 + v1 + v2;			/*In order to assure continuity of 5bytes(C, T1, L1, T2, L2) + correspondent V */
	
	unsigned char startpackage[startpackage_len];

	startpackage[0] = control_byte;
	startpackage[1] = CONTROLT1;
	startpackage[2] = v1;
 	*((off_t *)(startpackage[3])) = f_size; 		/*POSIX standard in which is assigned an integer in regards to a filesize*/
	startpackage[3 + v1] = CONTROLT2;
	startpackage[3 + v1 + 1] = v2; /* +1 from CONTROLT2 */

 	strcat((char *)startpackage + 5 + sizeof(f_information.st_size), al.filename);


	if(control_byte == CONTROLSTART){
		printf("\n||File: %s ||\n", al.filename);
		printf("||Size: %ld (bytes)||\n\n", f_size);
	}

	if (!llwrite(al.fileDescriptor, startpackage,startpackage_len)) {
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

void receiveControlPacket(int* file_length, char**filename) {
	unsigned char *read_package;
  unsigned int package_size = llread(al.fd, &read_package);

  if(package_size < 0) {
    perror("Couldn't read linklayer whilst receving package.");
	  exit(-1);
  }

  if(read_package[0] == CONTROLEND){ return; }  /*End of transfer process, nothing to process any further.*/

  int pck_index = 1;
  unsigned int n_bytes;

  for(int i = 0 ; i <= 1; i++){
    int pck_type = read_package[pck_index++];

    switch(pck_type){
      case CONTROLT1:
        n_bytes = (unsigned int) read_package[pck_index++];

        

        break;

      case CONTROLT2:
        n_bytes = (unsigned int) read_package[pck_index++];

      default:
      printf("File parameter couldn't be recognised, moving ahead...");
    }

  }
/*
		if(paramType == PARAM_FILE_SIZE){
			numBytes = (unsigned int) package[index++];

			char* length = (char*)malloc(numBytes);
			memcpy(length, &package[index], numBytes);

			*fileLength = atoi(length);
			free(length);
		}
		else if(paramType == PARAM_FILE_NAME){
			numBytes = (unsigned char) package[index++];
			memcpy(*fileName, &package[index], numBytes);
		}
		else
			printf("Unrecognised file parameter, skipping\n");

		index += numBytes;
}
*/
  return;
}

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
