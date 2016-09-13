
CC=gcc
BC=../basic
CFLAGS=-c -Wall -g
LDFLAGS=-g
SOURCES=test.bas
OBJECTS=$(SOURCES:.bas=.o)
EXECUTABLE=a.test
BASLIB=../lib/print.o

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(BASLIB) -o $@

%.o : %.s
	$(CC) $(CFLAGS) $< -o $@

%.s : %.bas
	$(BC) < $< > $@

tags: $(SOURCES)
	ctags $(SOURCES)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

test:
	./runtests

