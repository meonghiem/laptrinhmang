CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all:  tcp_server tcp_client

tcp_server: tcp_server.o
			${CC} -o tcp_server tcp_server.o ./libfdr/libfdr.a

tcp_server.o: tcp_server.c
			${CC} ${CFLAGS} tcp_server.c -I ./libfdr

tcp_client:	tcp_client.o 
		gcc -g -o tcp_client tcp_client.o  -lm
tcp_client.o:	tcp_client.c 
	gcc -g -c tcp_client.c -lm

clean:
	rm -f *.o *.exe