#include "linkLayer.h"

void retransmission(int signum) {
    ll.retransmit = TRUE;
}

void setSET() {
  ll.SET[0] = FLAG;
  ll.SET[1] = TRANSMITTERSA;
  ll.SET[2] = SETUP;
  ll.SET[3] = ll.SET[1] ^ ll.SET[2];
  ll.SET[4] = FLAG;
}

void setUAck(int status) {
  ll.UAck[0] = FLAG;

  if(status == TRANSMITTER) {
    ll.UAck[1] = TRANSMITTERSA;
  }
  else if(status==RECEIVER){
    ll.UAck[1] = RECEIVERSA;
  }

  ll.UAck[2] = UA;
  ll.UAck[3] = ll.UAck[1] ^ ll.UAck[2];
  ll.UAck[4] = FLAG;
}

void setDisc(int status) {
  ll.DISC[0] = FLAG;

  if(status == TRANSMITTER) {
    ll.DISC[1] = TRANSMITTERSA;
  }
  else if(status == RECEIVER) {
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

void setRR0(){
  ll.RR[2] = RR_CONTROL0;
  setRR();
}

void setRR1(){
  ll.RR[2] = RR_CONTROL1;
  setRR();
}

void setREJ(){
  ll.REJ[0] = FLAG;
  ll.REJ[1] = RECEIVERSA;
  ll.REJ[3] = ll.REJ[1] ^ ll.REJ[2];
  ll.REJ[4] = FLAG;
}

void setREJ0(){
  ll.REJ[2] = REJ_CONTROL0;
  setREJ();
}

void setREJ1(){
  ll.REJ[2] = REJ_CONTROL1;
}

void sendSFrame(int fd, unsigned char * frame, int triggerAlarm) {
    int res;

    //mandar trama de supervisão
    res = write(fd, frame, ll.frameSLength);

    if (res < 0) {
        perror("Writing S frame error");
        exit(-1);
    }

    printf("Frame sent (bytes: %d)\n", res);

    sleep(1);

    if(triggerAlarm) {
      alarm(ll.timeout);
    }
}

void receiveSFrame(int fd, int senderStatus, unsigned char controlByte, unsigned char * retransmit, unsigned int retransmitSize) {
  enum receiveStates {
    INIT,
    F,
    FA,
    FAC,
    FACBCC
  } receiveState;

  receiveState = INIT;
  int res;
  unsigned char byte, byteA;
  int unreceived = TRUE;

  while(unreceived) {

    if(retransmit != NULL) {
      if(ll.retransmit) {
        if (ll.numRetransmissions == 0) {
            printf("No more retransmissions, leaving.\n");
            exit(-1);
        }
        res = write(fd, retransmit, retransmitSize);
        printf("Frame sent again (bytes: %d)\n", res);
        ll.numRetransmissions--;
        alarm(ll.timeout);
        ll.retransmit = FALSE;
      }
    }

    res = read(fd, &byte, 1);
    if(res<0) {
      perror("Receiving reading error");
    }

    switch(receiveState) {
      case INIT:
          if (byte == FLAG)
              receiveState = F;
          break;

      case F:
          if(senderStatus == TRANSMITTER) {
            if(byte == TRANSMITTERSA) {
              receiveState = FA;
              byteA = byte;
              break;
            }
            else if(byte == FLAG) {
              receiveState = F;
              break;
            }
            else {
              receiveState = INIT;
              break;
            }
          }
          else if(senderStatus == RECEIVER) {
            if(byte == RECEIVERSA) {
              receiveState = FA;
              byteA = byte;
              break;
            }
            else if(byte == FLAG) {
              receiveState = F;
              break;
            }
            else {
              receiveState = INIT;
              break;
            }
          }

      case FA:
          if (byte == controlByte) {
            receiveState = FAC;
            break;
          }
          else if (byte == FLAG) {
            receiveState = F;
            break;
          }
          else {
            receiveState = INIT;
            break;
          }

      case FAC:
          if (byte == (byteA ^ controlByte)) {
            receiveState = FACBCC;
            break;
          }
          else if (byte == FLAG) {
            receiveState = F;
            break;
          }
          else {
            receiveState = INIT;
            break;
          }

      case FACBCC:
          if (byte == FLAG) {
              unreceived = FALSE;
              alarm(0);
              ll.numRetransmissions = ll.maxRetransmissions;
              ll.retransmit = FALSE;
              printf("Received frame\n");
              break;
          }
          else {
            receiveState = INIT;
            break;
          }

      default:
          break;
    }

  }

}

/* só termina quando ler rr ou quando acabarem as retransmissões */
void receiveRRREJ(int fd, unsigned char rr, unsigned char rej, unsigned char * retransmit, unsigned int retransmitSize) {
  enum receiveStates {
    INIT,
    F,
    FA,
    FAC,
    FACBCC
  } receiveState;

  receiveState = INIT;
  int res;
  unsigned char byte, controlByte;
  int unreceived = TRUE;

  while(unreceived) {

    if(retransmit != NULL) {
      if(ll.retransmit) {
        if (ll.numRetransmissions == 0) {
            printf("No more retransmissions, leaving.\n");
            exit(-1);
        }
        res = write(fd, retransmit, retransmitSize);
        printf("Frame sent again (bytes: %d)\n", res);
        ll.numRetransmissions--;
        alarm(ll.timeout);
        ll.retransmit = FALSE;
      }
    }

    res = read(fd, &byte, 1);
    if(res<0) {
      perror("Receiving reading error");
    }

    switch(receiveState) {
      case INIT:
          if (byte == FLAG)
              receiveState = F;
          break;

      case F:
          if(byte == RECEIVERSA) {
            receiveState = FA;
            break;
          }
          else if(byte == FLAG) {
            receiveState = F;
            break;
          }
          else {
            receiveState = INIT;
            break;
          }

      case FA:
          if (byte == rr) {
            controlByte = rr;
            receiveState = FAC;
            break;
          }
          else if(byte == rej) {
            controlByte = rej;
            ll.retransmit = TRUE;
            ll.numRetransmissions++;
            break;
          }
          else if (byte == FLAG) {
            receiveState = F;
            break;
          }
          else {
            receiveState = INIT;
            break;
          }

      case FAC:
          if (byte == (RECEIVERSA ^ controlByte)) {
            receiveState = FACBCC;
            break;
          }
          else if (byte == FLAG) {
            receiveState = F;
            break;
          }
          else {
            receiveState = INIT;
            printf("BCC error while reading\n");
            break;
          }

      case FACBCC:
          if (byte == FLAG) {
              unreceived = FALSE;
              alarm(0);
              ll.numRetransmissions = ll.maxRetransmissions;
              ll.retransmit = FALSE;
              printf("Received frame\n");
              break;
          }
          else {
            receiveState = INIT;
            break;
          }

      default:
          break;
    }

  }
}

int llopen(char* serialport, int status) {
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
    }
    else if (status == RECEIVER) {
        receiveSFrame(fd, TRANSMITTER, SETUP, NULL, 0);
        sendSFrame(fd, ll.UAck, FALSE);
    }

    return 0;
}

unsigned char * byteStuffing(unsigned char * frame, unsigned int * length) {
  unsigned char * stuffedFrame = (unsigned char *) malloc(*length);
  unsigned int finalLength = *length;

  int i, j = 0;
  stuffedFrame[j++] = FLAG;

  //excluir FLAG inicial e final
  for(i = 1; i < *length - 2; i++) {
    if(frame[i] == FLAG) {
      stuffedFrame = (unsigned char *) realloc(stuffedFrame, ++finalLength);
      stuffedFrame[j] = ESCAPE;
      stuffedFrame[++j] = PATTERNFLAG;
      j++;
      continue;
    }
    else if(frame[i] == ESCAPE) {
      stuffedFrame = (unsigned char *) realloc(stuffedFrame, ++finalLength);
      stuffedFrame[j] = ESCAPE;
      stuffedFrame[++j] = PATTERNESCAPE;
      j++;
      continue;
    }
    else {
      stuffedFrame[j++] = frame[i];
    }
  }

  stuffedFrame[j] = FLAG;

  *length = finalLength;

  return stuffedFrame;
}

unsigned char * byteDestuffing(unsigned char * data, unsigned int * length){
  unsigned int finalLength = 0;
  unsigned char * newData = malloc(finalLength);

  int i;
  for(i=0; i< *length; i++) {

    if(data[i] == ESCAPE) {
      if(data[i+1] == PATTERNFLAG) {
        newData = realloc(newData, ++finalLength);
        newData[finalLength - 1] = FLAG;
        continue;
      }
      else if(data[i+1] == PATTERNESCAPE) {
        newData = realloc(newData, ++finalLength);
        newData[finalLength-1] = ESCAPE;
        continue;
      }
    }

    else {
      newData = realloc(newData, ++finalLength);
      newData[finalLength - 1] = data[i];
    }

  }

  *length = finalLength;
  return newData;
}

/*
Pra mandar tramas i com a mensagem buffer no campo de dados
*/
int llwrite(int fd, unsigned char * buffer, unsigned int length) {
  unsigned int totalLength = 6 + length;
  unsigned char IFrame[totalLength], BCC2;

  IFrame[0] = FLAG;
  IFrame[1] = TRANSMITTERSA; // só o emissor chama a llwrite (o recetor não envia tramas I)

  if(ll.sequenceNumber == 0) {
    IFrame[2] = CONTROL0;
  }
  else if(ll.sequenceNumber == 1) {
    IFrame[2] = CONTROL1;
  }

  IFrame[3] = IFrame[1] ^ IFrame[2];

  int i;
  IFrame[4] = buffer[0];
  IFrame[5] = buffer[1];
  BCC2 = IFrame[4] ^ IFrame[5];
  for(i = 5; i < length + 4; i++) {
    IFrame[i] = buffer[i-4];
    BCC2 = BCC2 ^ IFrame[i];
  }

  IFrame[length-2] = BCC2;
  IFrame[length-1] = FLAG;

  unsigned char * stuffedFrame = byteStuffing(IFrame, &totalLength);
  int res = write(fd, stuffedFrame, totalLength);
  sleep(1);

  alarm(ll.timeout);

  if(ll.sequenceNumber == 0) {
    receiveRRREJ(fd, RR_CONTROL1, REJ_CONTROL0, stuffedFrame, totalLength);
    ll.sequenceNumber = 1;
  }
  else if(ll.sequenceNumber == 1) {
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
NOTA: buffer tem que ser alocado com malloc antes de chamar llread!!!
*/
int llread(int fd, unsigned char * buffer) {
    enum states {
        INIT,
        F,
        FA,
        FAC,
        FACBCCD,
        FACBCCDBCCF
    } state;
    state = INIT;

    unsigned char byte, controlByte;
    unsigned int sequenceNumber;
    int unreceived = TRUE;

    unsigned int length = 0;
    unsigned char * dbcc = (unsigned char *) malloc(length);
    unsigned char * destuffed;

    while (unreceived) {
        int res = read(fd, &byte, 1);

        if(res<0) {
          perror("Receiving reading error");
        }

        switch (state) {

        case INIT:
            if (byte == FLAG) {
              state = F;
              break;
            }

        case F:
            if (byte == TRANSMITTERSA) {
              state = FA;
              break;
            }
            else if (byte == FLAG) {
              state = F;
              break;
            }
            else {
              state = INIT;
              break;
            }

        case FA:
            if (byte == CONTROL0) {
              controlByte = CONTROL0;
              sequenceNumber = 0;
              state = FAC;
              break;
            }
            else if (byte == CONTROL1) {
              controlByte = CONTROL1;
              sequenceNumber = 1;
              state = FAC;
              break;
            }
            else if (byte == FLAG) {
              state = F;
              break;
            }
            else {
              state = INIT;
              break;
            }

        case FAC:
            if (byte == (TRANSMITTERSA ^ controlByte)) {
              state = FACBCCD;
              break;
            }
            else if(byte == FLAG) {
              state = F;
              break;
            }
            else {
              state = INIT;
              break;
            }

        case FACBCCD:
          if(byte == FLAG) {
            state = FACBCCDBCCF;
            destuffed = byteDestuffing(dbcc, &length);
            free(dbcc);
            if(!checkBCC(destuffed, length)) {
              if(sequenceNumber == 0) {
                setREJ0();
                sendSFrame(fd, ll.REJ, FALSE);
                return -1;
              }
              else if(sequenceNumber == 1) {
                setREJ1();
                sendSFrame(fd, ll.REJ, FALSE);
                return -1;
              }
            }

            break;
          }

          else {
            dbcc = realloc(dbcc, ++length);
            dbcc[length-1] = byte;
            break;
          }

        case FACBCCDBCCF:
          unreceived = FALSE;
          break;

        default:
          break;
        }
    }

        //copiar tudo exceto BCC2

    buffer = realloc(buffer, --length);

    int i;
    for(i=0; i<length; i++) {
      buffer[i] = destuffed[i];
    }

    if(sequenceNumber == 0) {
      setRR1();
      sendSFrame(fd, ll.RR, FALSE);
    }
    else if(sequenceNumber == 1) {
      setRR0();
      sendSFrame(fd, ll.RR, FALSE);
    }

    free(destuffed);
    return length;
}

/* data = D1.....Dn BCC2*/
int checkBCC(unsigned char * data, int length) {
    int i;
    unsigned char BCC2 = data[0];
                  //exluir BCC2 (ocupa 1 byte)
    for (i = 1; i < length - 1; i++) {
        BCC2 ^= data[i];
    }
                    //último elemento -> BCC2
    if (BCC2 == data[length - 1]) {
        return TRUE;
    }
    else
        return FALSE;
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

    if(status == TRANSMITTER) {
      setDisc(TRANSMITTER);
      sendSFrame(fd, ll.DISC, TRUE);

      receiveSFrame(fd, RECEIVER, C_DISC, ll.DISC, ll.frameSLength);

      setUAck(TRANSMITTER);
      sendSFrame(fd, ll.UAck, FALSE);
    }
    else if(status == RECEIVER) {
      receiveSFrame(fd, TRANSMITTER, C_DISC, NULL, 0);

      setDisc(RECEIVER);
      sendSFrame(fd, ll.DISC, TRUE);
      receiveSFrame(fd, TRANSMITTER, UA, ll.DISC, ll.frameSLength);
    }

    /*

    while (state != 5) {

        if (read(fd, &c, 1) == -1) {

            return -1;
        }



        if (c == FLAG&& state == 0)
            state = 1;
        else if (state == 1 && c == INPUTS_A)
            state = 2;
        else if (state = 2 && c == C_DISC)
            state = 3;
        else if (state = 3 && c == (INPUTS_A ^ C_DISC))
            state = 4;
        else if (state = 4 && c == FLAG)
            state = 5;
    }

    if (write(fd, DISC, 5) != 5) {

        return -1;
    }

    state = 0;

        while (state != 5)
    {

        if (read(fd, &c, 1) == -1) {

            return -1;
        }

        if (c == FLAG&& state == 0)
            state = 1;
        else if (state == 1 && c == INPUTS_A)
            state = 2;
        else if (state = 2 && c == C_SET)
            state = 3;
        else if (state = 3 && c == (INPUTS_A ^ C_SET))
            state = 4;
        else if (state = 4 && c == FLAG)
            state = 5;
    }
    */

    return 0;
}
