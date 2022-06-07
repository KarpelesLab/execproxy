#!/bin/make

CC=gcc
STRIP=strip
RM=rm
PREFIX=/usr/local/bin
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

.PHONY: install clean

install: $(TARGET)
	install -D -m0755 "$(TARGET)" "$(DESTDIR)$(PREFIX)"

clean:
	$(RM) $(TARGET) $(OBJECTS)
