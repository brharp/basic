
CC=gcc
CFLAGS=-c -Wall -g
LDFALGS=
SOURCES=print.s
OBJECTS=$(SOURCES:.s=.o)

all: $(OBJECTS)

tags: $(SOURCES)
	ctags $(SOURCES)

clean:
	rm -f $(OBJECTS)

