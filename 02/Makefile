CC = gcc
CFLAGS = -Wall

.PHONY: all, clean

all: bin/fash bin/summ bin/difference bin/strcompare bin/help

bin/fash: bin/fash.o
	$(CC) bin/fash.o -o bin/fash

bin/fash.o: src/fash.c
	$(CC) $(CFLAGS) -c src/fash.c

bin/summ: bin/summ.o
	$(CC) bin/summ.o -o bin/summ

bin/summ.o: src/summ.c
	$(CC) $(CFLAGS) -c src/summ.c

bin/difference: bin/difference.o
	$(CC) bin/difference.o -o bin/difference

bin/difference.o: src/difference.c
	$(CC) $(CFLAGS) -c src/difference.c

bin/strcompare: bin/strcompare.o
	$(CC) bin/strcompare.o -o bin/strcompare

bin/strcompare.o: src/strcompare.c
	$(CC) $(CFLAGS) -c src/strcompare.c
	
bin/help: bin/help.o
	$(CC) bin/help.o -o bin/help

bin/help.o: src/help.c
	$(CC) $(CFLAGS) -c src/help.c

clean:
	rm -rf bin/*.o
