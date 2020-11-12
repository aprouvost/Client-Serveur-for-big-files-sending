CFLAGS	+= -Wall   -g
LDFLAGS += -Wall

all: main

client.o: client.c
	gcc ${CFLAGS} -c client.c -o client.o

server.o: server.c
	gcc ${CFLAGS} -c server.c -o server.o

main: clean ../client.o server.o
	gcc ${LDFLAGS} client.o -o Client
	gcc ${LDFLAGS} server.o -o Server

clean:
	-rm -rf *.o Client Server
