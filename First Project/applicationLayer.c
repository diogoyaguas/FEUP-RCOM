#include "applicationLayer.h"

int sendData(char *filename) {

  if (sendControlPacket(filename, CONTROLSTART) < 0)
    return -1;

  int bytesRead = 0, seqNumber = 0;
  unsigned char *buffer;

  while ((bytesRead = fread(buffer, sizeof(char), packetSize, al.fileDescriptor)) > 0) {

    if (sendPacket(seqNumber, buffer, bytesRead) < 0)
      return -1;

    seqNumber++;
  }

  if (fclose(al.fileDescriptor) < 0) {
    printf("Error closing the file.\n");
    return -1;
  }

  if (sendControlPacket(filename, CONTROLEND) < 0)
    return -1;

  return 0;
}

int receiveData(char *filename) {

  if (receiveControlPacket() < 0)
    return -1;

    int bytesRead = 0, seqNumber = 0, counter = 0;;
  unsigned char *buffer;

  while(counter < fileSize) {

    bytesRead = receivePacket(&buffer, seqNumber);
    if(bytesRead < 0)
    return -1;

          counter+= bytesRead;
    fwrite(buffer, sizeof(char), bytesRead, al.fileDescriptor);
    seqNumber++;
    }

     if (fclose(al.fileDescriptor) < 0) {
    printf("Error closing the file.\n");
    return -1;
  }

  if (receiveControlPacket() < 0)
    return -1;

  return 0;
}

int sendControlPacket(char *filename, unsigned char control_byte) {
  struct stat f_information;

  if (fstat(al.fileDescripto, &f_information) < 0) {
    perror("Couldn't obtain information regarding the file.");
    return -1;
  }

  off_t f_size = f_information.st_size; /* total size, in signed bytes */
  unsigned int v1 = sizeof(f_size);
  unsigned int v2 = strlen(filename);

  int startpackage_len = 5 + v1 + v2; /*In order to assure continuity of
                                         5bytes(C, T1, L1, T2, L2) +
                                         correspondent V */

  unsigned char startpackage[startpackage_len];

  startpackage[0] = control_byte;
  startpackage[1] = CONTROLT1;
  startpackage[2] = v1;
  *((off_t *)(startpackage[3])) =
      f_size; /*POSIX standard in which is assigned an integer in regards to a
                 filesize*/
  startpackage[3 + v1] = CONTROLT2;
  startpackage[3 + v1 + 1] = v2; /* +1 from CONTROLT2 */

  strcat((char *)startpackage + 5 + sizeof(f_information.st_size), filename);

  if (control_byte == CONTROLSTART) {
    printf("\n||File: %s ||\n", filename);
    printf("||Size: %ld (bytes)||\n\n", f_size);
  }

  if (!llwrite(al.fd, startpackage, startpackage_len)) {
    printf("Couldn't write control package.\n");
   	return -1;
  }

  return 0;
}

int sendPacket(int seqNumber, char *buffer, int length) {

  int totalLength = length + 4;
  unsigned char dataPacket[totalLength];

  dataPacket[0] = CONTROLDATA;
  dataPacket[1] = seqNumber + '0';
  dataPacket[2] = length / 256;
  dataPacket[3] = length % 256;

  memcpy(&dataPacket[4], buffer, length);

  if (llwrite(al.fd, dataPacket, totalLength) < 0) {

    printf("Error sending data packet");
  }

  return 0;
}

int receiveControlPacket() {
  unsigned char *read_package;
  unsigned int package_size = llread(al.fd, &read_package);

  if (package_size < 0) {
    perror("Couldn't read linklayer whilst receving package.");
    return -1;
  }

  if (read_package[0] == CONTROLEND) {
    return -1;
  } /*End of transfer process, nothing to process any further.*/

  int pck_index = 1;
  unsigned int n_bytes;

  for (int i = 0; i <= 1; i++) {
    int pck_type = read_package[pck_index++];

    switch (pck_type) {
    case CONTROLT1:
      n_bytes = (unsigned int)read_package[pck_index++];
<
      al.file_data = (unsigned char*)malloc(n_bytes); /* Allocating file length not inicialized */
      memcpy(al.file_data, &read_package[pck_index], n_bytes);                       /* Transfering block of memory to file_data */>

      break;

    case CONTROLT2:
      n_bytes = (unsigned int)read_package[pck_index++];

      al.filename = (unsigned char*)malloc(n_bytes); /* Allocating filename memory block not inicialized */
      memcpy(al.filename, &read_package[pck_index], n_bytes);                       /* Transfering block of memory to a.layer's filename */>

    default:
      printf("File parameter couldn't be recognised, moving ahead...");
    }
      pck_index += numBytes;
  }

  return 0;
}

int receivePacket(unsigned char **buffer, int seqNumber) {

  unsigned char *information = NULL;
  int K = 0; // number of octets

  if (llread(al.fd, information) < 0) {

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
