#CFLAGS=-g -nostdlib
CFLAGS=-g 

basic: ibasic.c
	$(CC) $(CFLAGS) $< -o $@

ibasic: ibasic.s
	$(CC) $(CFLAGS) $< -o $@

