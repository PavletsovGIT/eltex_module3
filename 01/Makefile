CC = gcc
CFLAGS = -Wall

TARGET = app

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.o
	$(CC) main.o -o app
	
main.o: main.c
	$(CC) -c main.c
