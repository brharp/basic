
CC=gcc
CFLAGS=-c -Wall -g
LDFALGS=
#SOURCES=Main.c Input.c Output.c Error.c Parser.c Scanner.c CodeGen.c
SOURCES=basic.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=basic

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

test: test.bas
	./basic < test.bas > test.s
	$(CC) -g test.s

tags: $(SOURCES)
	ctags $(SOURCES)

.c.s:
	$(CC) -Wall -O0 -ansi -m64 -masm=intel -S $< -o $@

