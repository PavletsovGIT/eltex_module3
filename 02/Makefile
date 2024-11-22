CC = gcc
CFLAGS = - Wall

.PHONY: all, clean

all: fash summ difference strcompare help

fash: fash.o
	$(CC) fash.o -o fash

fash.o: fash.c
	$(CC) $(CFALGS) -c fash.c

summ: summ.o
	$(CC) summ.o -o summ

summ.o: summ.c
	$(CC) $(CFALGS) -c summ.c

difference: difference.o
	$(CC) difference.o -o difference

difference.o: difference.c
	$(CC) $(CFALGS) -c difference.c

strcompare: strcompare.o
	$(CC) strcompare.o -o strcompare

strcompare.o: strcompare.c
	$(CC) $(CFALGS) -c strcompare.c
	
help: help.o
	$(CC) fash.o -o fash

help.o: help.c
	$(CC) $(CFALGS) -c help.c

clean:
	rm *.o
