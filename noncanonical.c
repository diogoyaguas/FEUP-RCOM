/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "applicationLayer.h"
#include "linkLayer.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

/*
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define SETUP 0x03
#define INPUTS_A 0x03
#define A 0x01
#define UA 0x07
#define ESCAPE 0x7D
#define PATTERNFLAG 0x5E
#define PATTERNESCAPE 0x5D
#define CONTROL0 0x00
#define CONTROL1 0X40
#define RR_CONTROL0 0x05 // RR (receiver ready / positive ACK)
#define RR_CONTROL1 0x85
#define REJ_CONTROL0 0x01 // REJ (reject / negative ACK)
#define REJ_CONTROL1 0x81
#define C_DISC 0x0B
#define C_SET 0x03

volatile int STOP = FALSE;

void sendControlMessage(int fd, unsigned char C)
{
    unsigned char message[5];
    message[0] = FLAG;
    message[1] = A;
    message[2] = C;
    message[3] = message[1] ^ message[2];
    message[4] = FLAG;
    write(fd, message, 5);
}

int llread(int fd, char* buffer)
{
    // Reception of the packet
    enum states {
        INIT,
        F,
        FA,
        FAC,
        FACBCC,
        FACBCCF
    } state;
    state = INIT;

    unsigned char* message;
    unsigned char messageRead;
    int unreceived = 1;
    int packetType;
    int send = 0;

    unsigned char* charsRead = (unsigned char*)malloc(0);
    unsigned int* lengthOfCharsRead = 0;

    while (unreceived) {
        int res = read(fd, &message, 1);

        printf("read %x %d\n", message, res);

        switch (state) {
        case INIT:
            if (*message == FLAG)
                state = F;
            break;

        case F:
            if (*message == INPUTS_A)
                state = FA;
            else if (*message == FLAG)
                state = F;
            else
                state = INIT;
            break;

        case FA:
            if (*message == CONTROL0) {
                packetType = 0;
                messageRead = *message;
                state = FAC;
            }
            else if (*message == CONTROL1) {
                packetType = 1;
                messageRead = *message;
                state = FAC;
            }
            else if (*message == FLAG)
                state = F;
            else
                state = INIT;
            break;

        case FAC:
            if (*message == (INPUTS_A ^ messageRead))
                state = FACBCC;
            else
                state = INIT;
            break;

        case FACBCC:
            if (*message == FLAG) {
                if (checkBCC(charsRead, *lengthOfCharsRead)) {
                    if (packetType)
                        sendControlMessage(fd, RR_CONTROL0);
                    else
                        sendControlMessage(fd, RR_CONTROL1);

                    unreceived = 1;
                    send = 1;
                    printf("Send 'Receiver Ready', P: %d\n", packetType);
                }
                else {
                    if (packetType)
                        sendControlMessage(fd, REJ_CONTROL0);
                    else
                        sendControlMessage(fd, REJ_CONTROL1);

                    unreceived = 1;
                    send = 0;
                    printf("Send 'Reject' , P: %d\n", packetType);
                }
            }
            else if (*message == ESCAPE) {
                state = FACBCCF;
            }
            else {
                charsRead = (unsigned char*)realloc(message, ++(*lengthOfCharsRead));
                charsRead[*lengthOfCharsRead - 1] = *message;
            }
            break;

        case FACBCCF:

            if (byteStuffingMechanism(message, charsRead, lengthOfCharsRead) == -1) {

                perror("Non valid character after escape character");
                return -1;
            }

            state = FACBCC;
            break;
        }
    }

    return *lengthOfCharsRead;
}

int byteStuffingMechanism(unsigned char* message, unsigned char* charsRead,
    int* lengthOfCharsRead)
{

    if (*message == PATTERNFLAG) {
        charsRead = (unsigned char*)realloc(charsRead, ++(*lengthOfCharsRead));
        charsRead[*lengthOfCharsRead - 1] = FLAG;
    }
    else {
        if (*message == PATTERNESCAPE) {
            charsRead = (unsigned char*)realloc(charsRead, ++(*lengthOfCharsRead));
            charsRead[*lengthOfCharsRead - 1] = ESCAPE;
        }
        else {
            return -1;
        }
    }
}

int checkBBC(unsigned char* message, int sizeMessage)
{
    int i = 1;
    unsigned char BCC2 = message[0];
    for (; i < sizeMessage - 1; i++) {
        BCC2 ^= message[i];
    }
    if (BCC2 == message[sizeMessage - 1]) {
        return 1;
    }
    else
        return FALSE;
}
*/

int llclose(int fd)
{

    unsigned char c;
    int state = 0;

    while (state != 5) {

        if (read(fd, &c, 1) == -1) {

            return -1;
        }

        if (c == FLAG&& state = 0)
            state = 1;
        else if (state == 1 && c = INPUTS_A)
            state = 2;
        else if (state = 2 && c = C_DISC)
            state = 3;
        else if (state = 3 && c = (INPUTS_A ^ C_DISC))
            state = 4;
        else if (state = 4 && c == FLAG)
            state = 5;
    }

    if (write(fd, DISC, 5) != 5) {

        return -1;
    }

    state = 0

        while (state != 5)
    {

        if (read(fd, &c, 1) == -1) {

            return -1;
        }

        if (c == FLAG&& state = 0)
            state = 1;
        else if (state == 1 && c = INPUTS_A)
            state = 2;
        else if (state = 2 && c = C_SET)
            state = 3;
        else if (state = 3 && c = (INPUTS_A ^ C_SET))
            state = 4;
        else if (state = 4 && c == FLAG)
            state = 5;
    }

    return 0;
}

int main(int argc, char** argv)
{
    struct termios oldtio, newtio;

    al.status = RECEIVER;
    al.fileDescriptor = llopen(argv[1], al.status);

    ll.timeout = 3;
    ll.numRetransmissions = 3;
    ll.frameSLength = 5;
    ll.retransmit = FALSE;

    if ((argc < 2) || ((strcmp("/dev/ttyS0", argv[1]) != 0) && (strcmp("/dev/ttyS1", argv[1]) != 0))) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    if (tcgetattr(al.fileDescriptor, &oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 10; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 0; /* blocking read until 1 char received */

    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

    tcflush(al.fileDescriptor, TCIOFLUSH);

    if (tcsetattr(al.fileDescriptor, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    //estabelecer conexao

    establishConnection(al.fileDescriptor, al.status);

    tcsetattr(al.fileDescriptor, TCSANOW, &oldtio);
    close(al.fileDescriptor);
    return 0;
}
