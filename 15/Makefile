CC = gcc
CFLAGS = -Wall -g

.PHONY: all, clean

all: server.out client.out

server.out: tcpsrv.c
	$(CC) $(CFLAGS) tcpsrv.c -o server.out

client.out: tcpcli.c
	$(CC) $(CFLAGS) tcpcli.c -o client.out

clean:
	rm -rf *.out