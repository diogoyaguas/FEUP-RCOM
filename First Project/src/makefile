# Makefile for RCOM - Project 1

COMPILER_TYPE = gnu
CC = gcc

PROG = rcom
PORT = /dev/ttyS0
SRCS = applicationLayer.c linkLayer.c
MAIN_T = writer.c
MAIN_R = reader.c

CFLAGS= -Wall

clean:
	rm -f $(PROG)

t:
	$(CC) $(CFLAGS) $(MAIN_T) $(SRCS) -o $(PROG)

r:
	$(CC) $(CFLAGS) $(MAIN_R) $(SRCS) -o $(PROG)
