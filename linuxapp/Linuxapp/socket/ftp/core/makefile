CC=gcc
CFLAGS=-O -g

all:server client

server:server.o
	$(CC) $< -o $@ 
client:client.o
	$(CC) $< -o $@ 
server.o:server.c
	$(CC) $(CFLAGS) -c $< -o $@ 
client.o:client.c
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY:
clean:
	$(RM) *.o server client
