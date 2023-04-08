CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all:  server client

server: server.o
			${CC} -o server server.o

server.o: server.c
			${CC} ${CFLAGS} server.c

client:	client.o 
		gcc -g -o client client.o  -lm
client.o:	client.c 
	gcc -g -c client.c -lm

clean:
	rm -f *.o *.exe