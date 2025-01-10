CC = gcc
CFLAG = -std=c11 -Wall
LOG_FLAG = -DLOG_ON -DLOG_I

.PHONY: all clean

all: main.o lib13.o
	$(CC) main.o lib13.o -o prog.out

lib13.o : lib13.c
	$(CC) $(CFLAG) -c $(LOG_FLAG) lib13.c

main.o : main.c
	$(CC) $(CFLAG) -c $(LOG_FLAG) main.c

clean:
	rm -rf *.o *.out