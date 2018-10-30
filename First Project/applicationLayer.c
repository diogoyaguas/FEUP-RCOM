#include "applicationLayer.h"

void go() {
  establishConnection(al.fd, al.status);

  if(al.status == TRANSMITTER) {
    sendData();
  }
  else if(al.status == RECEIVER) {
    receiveData();
  }
}

int setFile() {
  printf("Name of the file to be transmitted: \n");
  scanf("%s", al.filename);
  printf("You entered: %s", al.filename);

  if((al.fileDescriptor = open(al.filename, O_RDONLY)) < 0) {
    perror("Error opening the file");
    return -1;
  }

  return 0;
}

int getFile(char * filename) {
  if((al.fileDescriptor = open(al.filename, O_CREAT|O_APPEND)) < 0) {
    perror("Error opening the file");
    return -1;
  }

  return 0;
}

int sendData() {

  if (sendControlPacket(CONTROLSTART) < 0) {
    printf("Error in sendControlPacket\n");
    return -1;
  }

  int bytesRead = 0, seqNumber = 0;
  unsigned char * buffer = (unsigned char *) malloc(al.fragmentSize + 1);

  while ((bytesRead = read(al.fileDescriptor, buffer, al.fragmentSize)) > 0) {

    if (sendPacket(seqNumber, buffer, bytesRead) < 0) {
      printf("Error in sendPacket\n");
      free(buffer);
      return -1;
    }

    seqNumber++;
    seqNumber %= 255;
  }

  if (close(al.fileDescriptor) < 0) {
    printf("Error closing the file.\n");
    free(buffer);
    return -1;
  }

  if (sendControlPacket(CONTROLEND) < 0) {
    printf("Error in sendControlPacket\n");
    return -1;
  }

  free(buffer);
  llclose(al.fd, al.status);
  printf("Finished\n");
  return 0;
}

int receiveData() {

  if (receiveControlPacket() < 0) {
    printf("error in receiveControlPacket\n");
    return -1;
  }

  int bytesRead = 0, seqNumber = 0, counter = 0;;
  unsigned char * buffer;

  while(counter < al.fileSize) {

    bytesRead = receivePacket(&buffer, seqNumber);
    if(bytesRead < 0) {
      printf("Error in receivePacket\n");
      return -1;
    }

    counter+= bytesRead;
    write(al.fileDescriptor, buffer, bytesRead);
    seqNumber++;
    free(buffer);
    }

  if (close(al.fileDescriptor) < 0) {
    printf("Error closing the file.\n");
    return -1;
  }

  if (receiveControlPacket() < 0) {
    printf("Error in receiveControlPacket\n");
    return -1;
  }

  free(al.filename);
  free(al.file_data);
  llclose(al.fd, al.status);
  printf("Finished\n");
  return 0;
}

int sendControlPacket(unsigned char control_byte) {

  if (control_byte == CONTROLSTART) {
    if(setFile() < 0) {
      printf("Error getting the file\n");
    }
  }

  struct stat f_information;

  if (fstat(al.fileDescriptor, &f_information) < 0) {
    perror("Couldn't obtain information regarding the file.");
    return -1;
  }

  off_t f_size = f_information.st_size; /* total size in bytes, in signed integer */
  unsigned int l1 = sizeof(f_size);
  unsigned int l2 = strlen(al.filename);

  int startpackage_len = 5 + l1 + l2;

  unsigned char startpackage[startpackage_len];

  startpackage[0] = control_byte; // CONTROLSTART or CONTROLEND
  startpackage[1] = CONTROLT1;
  startpackage[2] = l1;
  *((off_t *)(startpackage + 3)) = f_size;
  startpackage[3 + l1] = CONTROLT2;
  startpackage[3 + l1 + 1] = l2; /* +1 from CONTROLT2 */

  strcat((char *) startpackage + 5 + l1, al.filename);

  if (llwrite(al.fd, startpackage, startpackage_len) <= 0) {
    printf("Couldn't write control package.\n");
   	return -1;
  }

  return 0;
}

int sendPacket(int seqNumber, unsigned char * buffer, int length) {

  int totalLength = length + 4;
  unsigned char dataPacket[totalLength];

  dataPacket[0] = CONTROLDATA;
  dataPacket[1] = seqNumber + '0';
  dataPacket[2] = length / 256;
  dataPacket[3] = length % 256;

  memcpy(&dataPacket[4], buffer, length);

  if (llwrite(al.fd, dataPacket, totalLength) < 0) {
    printf("Error sending data packet");
    return -1;
  }

  return 0;
}

int receiveControlPacket() {
  unsigned char * read_package;
  unsigned int package_size = llread(al.fd, &read_package);

  if (package_size < 0) {
    perror("Couldn't read linklayer whilst receving package.");
    return -1;
  }

  if (read_package[0] == CONTROLEND) {
    free(read_package);
    return -1;
  } /*End of transfer process, nothing to process any further.*/

  int pck_index = 1; // because we already know C ([0])
  unsigned int n_bytes;

  for (int i = 0; i <= 1; i++) {
    int pck_type = read_package[pck_index++]; // read T

    switch (pck_type) {
    case CONTROLT1:
      n_bytes = (unsigned int)read_package[pck_index++];
      al.file_data = (unsigned char*)malloc(n_bytes); /* Allocating file length not inicialized */
      al.fileSize = n_bytes;
      memcpy(al.file_data, &read_package[pck_index], n_bytes); /* Transfering block of memory to file_data */
      break;

    case CONTROLT2:
      n_bytes = (unsigned int)read_package[pck_index++];
      al.filename = (char*)malloc(n_bytes); /* Allocating filename memory block not inicialized */
      memcpy(al.filename, &read_package[pck_index], n_bytes); /* Transfering block of memory to a.layer's filename */
      getFile(al.filename);

    default:
      printf("T parameter in start control packet couldn't be recognised, moving ahead...");
    }
      pck_index += n_bytes;
  }

  free(read_package);
  return 0;
}

int receivePacket(unsigned char ** buffer, int seqNumber) {

  unsigned char *information = NULL;
  int K = 0; // number of octets

  if (llread(al.fd, &information) < 0) {
    printf("error in llread\n");
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

  *buffer = (unsigned char *) malloc(K);
  memcpy((*buffer), &information[4], K);

  free(information);

  return K;
}
