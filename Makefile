
CC=gcc
CFLAGS=-c -Wall -g
LDFALGS=
SOURCES=basic.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=basic

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

tags: $(SOURCES)
	ctags $(SOURCES)

.c.s:
	$(CC) -Wall -O0 -ansi -m64 -masm=intel -S $< -o $@

clean:
	rm -f *.o

