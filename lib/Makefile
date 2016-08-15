
CC=gcc
CFLAGS=-c -Wall -g
LDFALGS=
SOURCES=print.s
OBJECTS=$(SOURCES:.s=.o)

all: $(OBJECTS)

%.o : %.s
	$(CC) $(CFLAGS) -o $@ $<

tags: $(SOURCES)
	ctags $(SOURCES)

clean:
	rm -f $(OBJECTS)

