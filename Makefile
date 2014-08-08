CC=gcc
CFLAGS=-Wall -Ofast -lcurl -fstack-protector-all
BINDIR=/usr/bin

googler2: googler2.c 
	$(CC) $(CFLAGS) -g -c *.c 
	$(CC) $(CFLAGS) -o googler2 *.o 
	rm *.o

clean:
	rm googler2
