CC = gcc
CFLAGS = -Wall

.PHONY: all clean

all: app compute

app: main.o
	$(CC) $(CFLAGS) main.o -o app

main.o: main.c
	$(CC) $(CFLAGS) -c main.c
	
compute: compute.o
	$(CC) $(CFLAGS) compute.o -o compute

compute.o: compute.c
	$(CC) $(CFLAGS) -c compute.c
	
clean:
	rm -rf *.o app compute
