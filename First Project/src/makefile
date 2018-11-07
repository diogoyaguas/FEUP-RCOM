# Makefile for RCOM - Project 1

COMPILER_TYPE = gnu
CC = gcc

PROG = rcom
SRCS = main.c utils.c applicationLayer.c linkLayer.c

CFLAGS= -Wall -g

$(PROG): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(PROG)

clean:
	rm -f $(PROG)
