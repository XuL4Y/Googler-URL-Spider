CC=gcc
CFLAGS=-Wall -Os
BINDIR=/usr/bin

googler2: googler2.c 
	$(CC) $(CFLAGS) -g -c *.c 
	$(CC) $(CFLAGS) -g -o googler2 *.o -lcurl -Os 
	rm *.o

clean:
	rm googler2
