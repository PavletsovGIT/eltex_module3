CC = gcc
CFLAGS = -Wall -g

.PHONY: all, clean

all: chat.out raw.out

chat.out: udp_chat.o srvclilib.o
	$(CC) udp_chat.o srvclilib.o -o chat.out

udp_chat.o: udp_chat.c 
	$(CC) $(CFLAGS) -c udp_chat.c

srvclilib.o: srvclilib.c 
	$(CC) $(CFLAGS) -c srvclilib.c

raw.out: raw_sock.c
	$(CC) $(CFLAGS) raw_sock.c -o raw.out

clean:
	rm -rf *.o *.out