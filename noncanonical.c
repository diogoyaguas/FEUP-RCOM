/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define SETUP 0x03
#define INPUTS_A 0x03
#define A 0x01
#define UA 0x07

volatile int STOP=FALSE;

int llread(int fd, char * buffer) {

    //Reception of the packet
    enum states {INIT, F, FA, FAC, FACBCC, FACBCCF} state;
    state = INIT;

    unsigned char message;
    int unreceived = 1;
    int packetType;

    while(unreceived) {
        res = read(fd, &message, 1);
        printf("read %x %d\n", message, res);
        switch(state) {
        case INIT:
            if (message==FLAG)
                state = F;
            break;

        case F:
            if(message==INPUTS_A)
                state = FA;
            else if(message==FLAG)
                state = F;
            else state = INIT;
            break;

        case FA:
            if(message== 0x00) {
                packetType = 0;
                messageRead = message;
                state = FAC;

            } else if(message == 0x40)  {

                packetType = 1;
                messageRead = message;
                state =  FAC;
            }


            else if(message==FLAG)
                state=F;
            else state = INIT;
            break;

        case FAC:
            if(message == (INPUTS_A^ messageRead))
                state=FACBCC;
            else if(message==FLAG)
                state=F;
            else state = INIT;
            break;

        case FACBCC:
            if(message == FLAG) {
                state=FACBCCF;
                unreceived = 0;
            }
            else state=INIT;
            break;

        default:
            break;
        }
    }


    return -1;

}

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255], bufread[255];
    int i, sum = 0, speed = 0;
    char echo[255];

    strcpy(echo,"");

    if ( (argc < 2) ||
            ((strcmp("/dev/ttyS0", argv[1])!=0) &&
             (strcmp("/dev/ttyS1", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }


    /*
      Open serial port device for reading and writing and not as controlling tty
      because we don't want to get killed if linenoise sends CTRL-C.
    */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {
        perror(argv[1]);
        exit(-1);
    }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 10;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 char received */

    /*
      VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
      leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");



    //trama após receção
    int s_length = 5;
    unsigned char SET[s_length];
    SET[0] = FLAG;
    SET[4] = FLAG;
    SET[1] = A;
    SET[2] = UA;
    SET[3] = SET[1] ^ SET[2];

    res = write(fd, SET, s_length);
    printf("Sent packet \n", res);
    sleep(2);

    while (STOP==FALSE) {       /* loop for input */
        res = read(fd,buf,1);    /* returns after 5 chars have been input */
        buf[res]=0;               /* so we can printf... */
        if (buf[res-1]=='\0') STOP=TRUE;
        strcat(echo, buf);
    }

    res = write(fd, echo, strlen(echo) + 1);
    printf("%d bytes echoed\n", res);
    sleep(2);

    /*
      O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o
    */

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
