CC = gcc
CFLAGS = -Wall -g

.PHONY: all, clean

all: server.out client.out

pp.o: pp.c
	$(CC) $(CFLAGS) -c pp.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

server.out: server.o pp.o
	$(CC) server.o pp.o -o server.out

client.out: client.o pp.o
	$(CC) client.o pp.o -o client.out

clean:
	rm -rf *.o *.out