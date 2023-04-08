CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: main

main: main.o
			${CC} -o main main.o ./libfdr/libfdr.a

main.o: main.c
			${CC} ${CFLAGS} main.c -I ./libfdr

clean:
	rm -f *.o *.exe *~