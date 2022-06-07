#!/bin/make

CC=gcc
STRIP=strip
RM=rm
OBJECTS=main.o
CFLAGS=-pipe -Wall -O2 --std=gnu99
LDFLAGS=-static
TARGET=execproxy

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
	$(STRIP) $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJECTS)
