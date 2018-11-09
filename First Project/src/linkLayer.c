#include "linkLayer.h"

void retransmission(int signum) { ll.retransmit = TRUE; }

void setSET() {
  ll.SET[0] = FLAG;
  ll.SET[1] = TRANSMITTERSA;
  ll.SET[2] = SETUP;
  ll.SET[3] = ll.SET[1] ^ ll.SET[2];
  ll.SET[4] = FLAG;
}

void setUAck(int status) {
  ll.UAck[0] = FLAG;

  if (status == TRANSMITTER) {
    ll.UAck[1] = TRANSMITTERSA;
  } else if (status == RECEIVER) {
    ll.UAck[1] = RECEIVERSA;
  }

  ll.UAck[2] = UA;
  ll.UAck[3] = ll.UAck[1] ^ ll.UAck[2];
  ll.UAck[4] = FLAG;
}

void setDisc(int status) {
  ll.DISC[0] = FLAG;

  if (status == TRANSMITTER) {
    ll.DISC[1] = TRANSMITTERSA;
  } else if (status == RECEIVER) {
    ll.DISC[1] = RECEIVERSA;
  }

  ll.DISC[2] = C_DISC;
  ll.DISC[3] = ll.DISC[1] ^ ll.DISC[2];
  ll.DISC[4] = FLAG;
}

void setRR() {
  ll.RR[0] = FLAG;
  ll.RR[1] = RECEIVERSA;
  ll.RR[3] = ll.RR[1] ^ ll.RR[2];
  ll.RR[4] = FLAG;
}

void setRR0() {
  ll.RR[2] = RR_CONTROL0;
  setRR();
}

void setRR1() {
  ll.RR[2] = RR_CONTROL1;
  setRR();
}

void setREJ() {
  ll.REJ[0] = FLAG;
  ll.REJ[1] = RECEIVERSA;
  ll.REJ[3] = ll.REJ[1] ^ ll.REJ[2];
  ll.REJ[4] = FLAG;
}

void setREJ0() {
  ll.REJ[2] = REJ_CONTROL0;
  setREJ();
}

void setREJ1() { ll.REJ[2] = REJ_CONTROL1; }

void sendSFrame(int fd, unsigned char *frame, int triggerAlarm) {
  int res;

  // mandar trama de supervisão
  res = write(fd, frame, ll.frameSLength);

  if (res < 0) {
    perror("Writing S frame error");
    exit(-1);
  }

  printf("\nFrame sent (bytes: %d)\n", res);

  if (triggerAlarm) {
    alarm(ll.timeout);
  }
}

void receiveSFrame(int fd, int senderStatus, unsigned char controlByte,
                   unsigned char *retransmit, unsigned int retransmitSize) {
  enum receiveStates { INIT, F, FA, FAC, FACBCC } receiveState;

  receiveState = INIT;
  int res;
  unsigned char byte, byteA;
  int unreceived = TRUE;

  while (unreceived) {

    if (retransmit != NULL) {
      if (ll.retransmit) {
        if (ll.numRetransmissions == 0) {
          printf("\nNo more retransmissions, leaving...\n");
          exit(-1);
        }
        res = write(fd, retransmit, retransmitSize);
        printf("\nFrame sent again (bytes: %d)\n", res);
        ll.numRetransmissions--;
        alarm(ll.timeout);
        ll.retransmit = FALSE;
      }
    }

    res = read(fd, &byte, 1);
    if (res < 0) {
      perror("Receiving reading error");
    }
      switch (receiveState) {
      case INIT:
        if (byte == FLAG)
          receiveState = F;
        break;

      case F:
        if (senderStatus == TRANSMITTER) {
          if (byte == TRANSMITTERSA) {
            receiveState = FA;
            byteA = byte;
          } else if (byte == FLAG) {
            receiveState = F;
          } else
            receiveState = INIT;
        } else if (senderStatus == RECEIVER) {
          if (byte == RECEIVERSA) {
            receiveState = FA;
            byteA = byte;
          } else if (byte == FLAG) {
            receiveState = F;
          } else
            receiveState = INIT;
        }
        break;

      case FA:
        if (byte == controlByte) {
          receiveState = FAC;
        } else if (byte == FLAG) {
          receiveState = F;
        } else
          receiveState = INIT;
        break;
      case FAC:
        if (byte == (byteA ^ controlByte)) {
          receiveState = FACBCC;
        } else if (byte == FLAG) {
          receiveState = F;
        } else
          receiveState = INIT;
        break;

      case FACBCC:
        if (byte == FLAG) {
          unreceived = FALSE;
          alarm(0);
          ll.numRetransmissions = ll.maxRetransmissions;
          ll.retransmit = FALSE;
          printf("Received frame\n");
        } else
          receiveState = INIT;
        break;

      default:
        break;
      }
  }
}

/* só termina quando ler rr ou quando acabarem as retransmissões */
void receiveRRREJ(int fd, unsigned char rr, unsigned char rej,
                  unsigned char *retransmit, unsigned int retransmitSize) {
  enum receiveStates { INIT, F, FA, FAC, FACBCC } receiveState;

  receiveState = INIT;
  int res;
  unsigned char byte, controlByte;
  int unreceived = TRUE;

  while (unreceived) {

    if (retransmit != NULL) {
      if (ll.retransmit) {
        if (ll.numRetransmissions == 0) {
          printf("No more retransmissions, leaving.\n");
          exit(-1);
        }
        res = write(fd, retransmit, retransmitSize);
        printf("\nFrame sent again (bytes: %d)\n", res);
        ll.numRetransmissions--;
        alarm(ll.timeout);
        ll.retransmit = FALSE;
      }
    }

    res = read(fd, &byte, 1);
    if (res < 0) {
      perror("Receiving reading error");
    } else if (res == 0) {
        res = write(fd, retransmit, retransmitSize);
    	res = read(fd, &byte, 1);
	}

	//printf("%x ", byte);

      switch (receiveState) {
      case INIT:
        if (byte == FLAG)
          receiveState = F;
        break;

      case F:
        if (byte == RECEIVERSA) {
          receiveState = FA;
        } else if (byte == FLAG) {
          receiveState = F;
        } else
          receiveState = INIT;
        break;

      case FA:
        if (byte == rr) {
          controlByte = rr;
          receiveState = FAC;
          alarm(0);
        } else if (byte == rej) {
          controlByte = rej;
          ll.retransmit = TRUE;
          ll.numRetransmissions++;
        } else if (byte == FLAG) {
          receiveState = F;
        } else
          receiveState = INIT;
        break;

      case FAC:
        if (byte == (RECEIVERSA ^ controlByte)) {
          receiveState = FACBCC;
        } else if (byte == FLAG) {
          receiveState = F;
        } else {
          receiveState = INIT;
          printf("BCC error while reading\n");
        }
        break;
      case FACBCC:
        if (byte == FLAG) {
          unreceived = FALSE;
          alarm(0);
          ll.numRetransmissions = ll.maxRetransmissions;
          ll.retransmit = FALSE;
          if (controlByte == rr) {
            st.rrRcvd++;
            printf("Received RR frame\n");
          } else if (controlByte == rej) {
            st.rejRcvd++;
            printf("Received REJ frame\n");
          }
        } else
          receiveState = INIT;
        break;

      default:
        break;
      }
  }
}

int llopen(char *serialport, int status) {
  int fd;
  fd = open(serialport, O_RDWR | O_NOCTTY);

  if (fd < 0) {
    perror("Error opening serial port");
    exit(-1);
  }

  signal(SIGALRM, retransmission);

  return fd;
}

int establishConnection(int fd, int status) {
  setSET();
  setUAck(RECEIVER);

  if (status == TRANSMITTER) {
    sendSFrame(fd, ll.SET, TRUE);
    receiveSFrame(fd, RECEIVER, UA, ll.SET, ll.frameSLength);
  } else if (status == RECEIVER) {
    receiveSFrame(fd, TRANSMITTER, SETUP, NULL, 0);
    sendSFrame(fd, ll.UAck, FALSE);
  }

  printf("\nEstablished connection. Moving on to packets\n\n");

  return 0;
}

unsigned char *byteStuffing(unsigned char *frame, unsigned int *length) {
  unsigned char *stuffedFrame = (unsigned char *)malloc(*length);
  unsigned int finalLength = *length;

  int i, j = 0;
  stuffedFrame[j++] = FLAG;

  // excluir FLAG inicial e final
  for (i = 1; i < *length - 1; i++) {
    if (frame[i] == FLAG) {
      stuffedFrame = (unsigned char *)realloc(stuffedFrame, ++finalLength);
      stuffedFrame[j] = ESCAPE;
      stuffedFrame[++j] = PATTERNFLAG;
      j++;
      continue;
    } else if (frame[i] == ESCAPE) {
      stuffedFrame = (unsigned char *)realloc(stuffedFrame, ++finalLength);
      stuffedFrame[j] = ESCAPE;
      stuffedFrame[++j] = PATTERNESCAPE;
      j++;
      continue;
    } else {
      stuffedFrame[j++] = frame[i];
    }
  }

  stuffedFrame[j] = FLAG;

  *length = finalLength;

  return stuffedFrame;
}

unsigned char *byteDestuffing(unsigned char *data, unsigned int *length) {
  unsigned int finalLength = 0;
  unsigned char *newData = malloc(finalLength);

  int i;
  for (i = 0; i < *length; i++) {

    if (data[i] == ESCAPE) {
      if (data[i + 1] == PATTERNFLAG) {
        newData = (unsigned char *)realloc(newData, ++finalLength);
        newData[finalLength - 1] = FLAG;
        i++;
        continue;
      } else if (data[i + 1] == PATTERNESCAPE) {
        newData = (unsigned char *)realloc(newData, ++finalLength);
        newData[finalLength - 1] = ESCAPE;
        i++;
        continue;
      }
    }

    else {
      newData = (unsigned char *)realloc(newData, ++finalLength);
      newData[finalLength - 1] = data[i];
    }
  }

  *length = finalLength;
  return newData;
}

int randomError() {

  return ((random()%10) == 0) ? 1 : 0;
}

/*
Pra mandar tramas i com a mensagem buffer no campo de dados
*/
int llwrite(int fd, unsigned char *buffer, unsigned int length) {
  unsigned int totalLength = 6 + length;
  unsigned char IFrame[totalLength], BCC2, oldBCC;

  IFrame[0] = FLAG;
  IFrame[1] = TRANSMITTERSA; // só o emissor chama a llwrite (o recetor não
                             // envia tramas I)

  if (ll.sequenceNumber == 0) {
    IFrame[2] = CONTROL0;
  } else if (ll.sequenceNumber == 1) {
    IFrame[2] = CONTROL1;
  }

  IFrame[3] = IFrame[1] ^ IFrame[2];

  int i;
  IFrame[4] = buffer[0];
  BCC2 = IFrame[4];
  for (i = 5; i < length + 4; i++) {
    IFrame[i] = buffer[i - 4];
    BCC2 = BCC2 ^ IFrame[i];
  }

  IFrame[totalLength - 2] = BCC2;
  IFrame[totalLength - 1] = FLAG;

  unsigned char *stuffedFrame = byteStuffing(IFrame, &totalLength);

  oldBCC = stuffedFrame[totalLength - 2];
  stuffedFrame[totalLength - 2] += randomError();
  int res = write(fd, stuffedFrame, totalLength);
  printf("\nllwrite: sent I frame\n");

  stuffedFrame[totalLength - 2] = oldBCC;

  alarm(ll.timeout);

  if (ll.sequenceNumber == 0) {
    receiveRRREJ(fd, RR_CONTROL1, REJ_CONTROL0, stuffedFrame, totalLength);
    ll.sequenceNumber = 1;
  } else if (ll.sequenceNumber == 1) {
    receiveRRREJ(fd, RR_CONTROL0, REJ_CONTROL1, stuffedFrame, totalLength);
    ll.sequenceNumber = 0;
  }

  free(stuffedFrame);

  ll.retransmit = FALSE;
  ll.numRetransmissions = ll.maxRetransmissions;

  return res;
}

/*
Para ler tramas i
retornar nr de caracteres lidos
colocar no buffer caracteres lidos
*/
int llread(int fd, unsigned char **buffer) {
  enum states { INIT, F, FA, FAC, FACBCCD, FACBCCDBCCF } state;
  state = INIT;

  int i;

  unsigned char byte, controlByte;
  int unreceived = TRUE;

  unsigned int length = 0;
  unsigned char *dbcc = (unsigned char *)malloc(length);
  *buffer = (unsigned char *)malloc(0);
  unsigned char *destuffed;

  while (unreceived) {
    int res = read(fd, &byte, 1);

    if (res < 0) {
      perror("llread: receiving reading error");
    } else if (res > 0) {

      switch (state) {

      case INIT:
        if (byte == FLAG)
          state = F;
        break;
      case F:
        if (byte == TRANSMITTERSA) {
          state = FA;
        } else if (byte == FLAG)
          state = F;
         else state = INIT;
        break;
      case FA:
        if (byte == CONTROL0 && ll.sequenceNumber == 0) {
          controlByte = CONTROL0;
          state = FAC;
        } else if (byte == CONTROL1 && ll.sequenceNumber == 1) {
          controlByte = CONTROL1;
          state = FAC;
        } else if (byte == FLAG)
          state = F;
         else state = INIT;
        break;
      case FAC:
        if (byte == (TRANSMITTERSA ^ controlByte)) {
          state = FACBCCD;
        } else if (byte == FLAG)
          state = F;
         else state = INIT;
        break;

      case FACBCCD:
        if (byte == FLAG) {

          destuffed = byteDestuffing(dbcc, &length);

          if (!checkBCC(destuffed, length)) {
            printf("llread: sending REJ\n");
            st.rejSent++;
            if (ll.sequenceNumber == 0) {
              setREJ0();
              sendSFrame(fd, ll.REJ, FALSE);
            } else if (ll.sequenceNumber == 1) {
              setREJ1();
              sendSFrame(fd, ll.REJ, FALSE);
            }
            length = 0;
            dbcc = (unsigned char *) realloc(dbcc, length);
            state = INIT;
            break;
          } else {
            state = FACBCCDBCCF;
            break;
          }
        } else {
          dbcc = (unsigned char *)realloc(dbcc, ++length);
          dbcc[length - 1] = byte;
        }
        break;

      case FACBCCDBCCF:
        unreceived = FALSE;
        break;

      default:
        break;
      }
    }
  }

  // copiar tudo exceto BCC2

  *buffer = (unsigned char *)realloc(*buffer, --length);

  for (i = 0; i < length; i++) {
    (*buffer)[i] = destuffed[i];
  }

  printf("llread: sending RR\n");
  st.rrSent++;
  if (ll.sequenceNumber == 0) {
    setRR1();
    sendSFrame(fd, ll.RR, FALSE);
    ll.sequenceNumber = 1;
  } else if (ll.sequenceNumber == 1) {
    setRR0();
    sendSFrame(fd, ll.RR, FALSE);
    ll.sequenceNumber = 0;
  }

  free(dbcc);
  free(destuffed);
  return length;
}

/* data = D1.....Dn BCC2*/
int checkBCC(unsigned char *data, int length) {
  int i;
  unsigned char BCC2 = data[0];
  // exluir BCC2 (ocupa 1 byte)
  for (i = 1; i < length - 1; i++) {
    BCC2 ^= data[i];
  }
  //último elemento -> BCC2
  if (BCC2 == data[length - 1]) {
    return TRUE;
  } else {
    printf("BCC2 doesn't check\nBCC2: %x, real BCC2: %x\n", data[length - 1],
           BCC2);
    return FALSE;
  }
}

void sendControlMessage(int fd, unsigned char c) {
  unsigned char message[5];
  message[0] = FLAG;
  message[1] = RECEIVERSA;
  message[2] = c;
  message[3] = message[1] ^ message[2];
  message[4] = FLAG;
  write(fd, message, 5);
}

int llclose(int fd, int status) {

  if (status == TRANSMITTER) {
    setDisc(TRANSMITTER);
    sendSFrame(fd, ll.DISC, TRUE);

    receiveSFrame(fd, RECEIVER, C_DISC, ll.DISC, ll.frameSLength);

    setUAck(TRANSMITTER);
    sendSFrame(fd, ll.UAck, FALSE);
  } else if (status == RECEIVER) {
    receiveSFrame(fd, TRANSMITTER, C_DISC, NULL, 0);

    setDisc(RECEIVER);
    sendSFrame(fd, ll.DISC, TRUE);
    receiveSFrame(fd, TRANSMITTER, UA, ll.DISC, ll.frameSLength);
  }

  return 0;
}
