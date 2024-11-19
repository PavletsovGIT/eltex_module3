CC = gcc
CFLAGS = - Wall

TARGET = app

.PHONY: .all .clean

all: $(TARGET)

$(TARGET): main.o contacts.o
	gcc main.o contacts.o -o $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

#cvslib.o: cvslib.c
	#$(CC) $(CFLAGS) -c cvslib.c

contacts.o: contacts.c
	$(CC) $(CFLAGS) -c contacts.c

clean:
	rm *.o
