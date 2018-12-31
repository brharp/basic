
CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=
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

i: interp.s
	$(CC) $(LDFLAGS) $<

clean:
	rm -f *.o

test:
	$(MAKE) -C t test

run:
	./basic < test.bas > test.s
	$(CC) -g test.s lib/print.o
	./a.out

%.s: %.bas
	./basic < $< > $@

%.exe: %.s
	$(CC) -g $< lib/*.o -o $@

