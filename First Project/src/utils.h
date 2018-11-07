#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define TRANSMITTER 1
#define RECEIVER 0
#define FALSE 0
#define TRUE 1

int getPacketSize();

int IsPowerOfTwo(int x);

int getBaudrate();

int getBaudrateNumber(int choice);