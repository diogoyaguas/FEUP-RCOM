/*Non-Canonical Input Processing*/
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
 
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define SETUP 0x03
#define A 0x03
#define RECEPTORSA 0x01
#define UA 0x07
 
volatile int STOP=FALSE;
volatile int retransmit = FALSE;

struct applicationLayer {
	int fileDescriptor;
};

struct linkLayer {
	char port[20];
	int baudRate;
	unsigned int sequenceNumber;
	unsigned int timeout;
	unsigned int numTransmissions;
	char frame[255];
};

void retransmission(int signum){
	retransmit = TRUE;
	printf("alarm\n");
}
 
int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    unsigned char buf[255], bufread[255];
    int i, sum = 0, speed = 0;
	struct linkLayer ll;

	ll.timeout = 3;
	ll.numTransmissions = 3;
	signal(SIGALRM, retransmission);  

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS0", argv[1])!=0) &&
          (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS0\n");
      exit(1);
    }
 
 
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
 
 
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }
 
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
 
    printf("New termios structure set\n Establishing conection\n");
 
    //trama	

    int s_length = 5;
    unsigned char SET[s_length];
    SET[0] = FLAG;
    SET[4] = FLAG;
    SET[1] = A;
    SET[2] = SETUP;
    SET[3] = SET[1] ^ SET[2];
 
    enum states {INIT, F, FA, FAC, FACBCC, FACBCCF} state;
    state = INIT;
	char byte;
 
    //mandar trama de supervisão
    res = write(fd, SET, s_length);
    printf("Packet sent (bytes: %d)\n", res);

	sleep(1);

	alarm(3);

    int unreceived = TRUE;
    while(unreceived) {

	  if(retransmit) {
		printf("Retransmit\n");
		if(ll.numTransmissions == 0) {
			printf("No more retransmissions, leaving.\n");
			exit(1);
		}
	  	res = write(fd, SET, s_length);
    	printf("Packet sent again (bytes: %d)\n", res);
		state = INIT;
		ll.numTransmissions--;
		alarm(3);
		retransmit = FALSE;
	  }

      res = read(fd, &byte, 1);

	  if(res<0){
		perror("read error");
	  }
	  else if (res==1){  
     	 bufread[res]=0;           
      	 printf(":%x:%d\n", byte, res);
	  }
	
      switch(state) {
        case INIT:
          if (byte==FLAG){
            state = F;
			printf("init\n");
          }
          break;
        case F:
          if(byte==RECEPTORSA)
            state = FA;
          else if(byte==FLAG)
            state = F;
          else state = INIT;
          break;
        case FA:
          if(byte==UA)
            state=FAC;
          else if(byte==FLAG)
            state=F;
          else state = INIT;
          break;
        case FAC:
          if(byte== (RECEPTORSA^UA))
            state=FACBCC;
          else if(byte==FLAG)
            state=F;
          else state = INIT;
          break;
        case FACBCC:
          if(byte==FLAG) {
            state=FACBCCF;
            unreceived = FALSE;
			alarm(0);
			ll.numTransmissions = 3;
          }
          else state=INIT;
          break;
        default: break;
      }
    }
 
    printf("Connection established, write something \n");
 

/*    fgets(buf);
 
    res = write(fd,buf,strlen(buf) + 1);
    printf("%d bytes written\n", res);
 
    sleep(2);
 
    while (STOP==FALSE) {     
      res = read(fd,bufread,1);   
      bufread[res]=0;           
      printf(":%s:%d\n", bufread, res);
      if (bufread[res-1]=='\0') STOP=TRUE;
    } 
*/
 
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
 
    close(fd);
    return 0;
}
