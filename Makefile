CC=gcc
CFLAGS=-Wall -Ofast -mtune=native -march=native -lcurl -fstack-protector-all -D_FORTIFY_SOURCE=1 -Wl,-z,relro,-z,now 
BINDIR=/usr/bin

googler2: googler2.c 
	$(CC) $(CFLAGS) -g -c *.c 
	$(CC) $(CFLAGS) -o googler2 *.o 
	rm *.o

clean:
	rm googler2
