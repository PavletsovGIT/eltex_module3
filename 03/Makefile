CC = gcc
CFLAGS = -Wall -g

TARGET = a.out

.PHONY: .all .clean

all: $(TARGET)

$(TARGET): main.o contacts.o
	gcc main.o contacts.o -o $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

contacts.o: contacts.c
	$(CC) $(CFLAGS) -c contacts.c

clean:
	rm -rf *.o
