CC=gcc
CFLAGS=-Wall -Wextra -Os
BINDIR=/usr/bin

0d1n: googler2.c 
	$(CC) $(CFLAGS) -g -c *.c 
	$(CC) $(CFLAGS) -g -o googler2 *.o -lcurl -Os 
	rm *.o

clean:
	rm googler2
