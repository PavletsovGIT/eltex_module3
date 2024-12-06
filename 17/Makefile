CC = gcc
CFLAGS = -Wall -g

.PHONY: all, clean

all: chat.out

chat.out: udp_chat.o srvclilib.o
	$(CC) udp_chat.o srvclilib.o -o chat.out

udp_chat.o: udp_chat.c 
	$(CC) $(CFLAGS) -c udp_chat.c

srvclilib.o: srvclilib.c 
	$(CC) $(CFLAGS) -c srvclilib.c

clean:
	rm -rf *.o *.out