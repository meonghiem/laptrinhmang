CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all:  server_side_udp client_side_udp

server_side_udp: server_side_udp.o
			${CC} -o server_side_udp server_side_udp.o ./libfdr/libfdr.a

server_side_udp.o: server_side_udp.c
			${CC} ${CFLAGS} server_side_udp.c -I ./libfdr

client_side_udp:	client_side_udp.o 
		gcc -g -o client_side_udp client_side_udp.o  -lm
client_side_udp.o:	client_side_udp.c 
	gcc -g -c client_side_udp.c -lm

clean:
	rm -f *.o *.exe