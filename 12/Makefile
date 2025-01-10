CC = gcc
CFLAG = -std=c11 -Wall
LOG_FLAG = -DLOG_ON -DLOG_I

.PHONY: all clean

all: main.o lib12.o
	$(CC) main.o lib12.o -o prog.out

lib12.o : lib12.c
	$(CC) $(CFLAG) -c $(LOG_FLAG) lib12.c

main.o : main.c
	$(CC) $(CFLAG) -c $(LOG_FLAG) main.c

clean:
	rm -rf *.o *.out