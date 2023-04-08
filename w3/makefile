all:	client_side_udp server_side_udp
client_side_udp:	client_side_udp.o 
		gcc -g -o client_side_udp client_side_udp.o  -lm
client_side_udp.o:	client_side_udp.c 
	gcc -g -c client_side_udp.c -lm
server_side_udp:	server_side_udp.o 
		gcc -g -o server_side_udp server_side_udp.o  -lm
server_side_udp.o:	server_side_udp.c 
	gcc -g -c server_side_udp.c -lm
clean:	
	rm -f *.o *.exe *~