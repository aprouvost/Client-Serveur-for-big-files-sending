CFLAGS	+= -Wall   -g
LDFLAGS += -Wall

all: main

common.o: src/common/common.h src/common/common.c
	gcc ${CFLAGS} -c src/common/common.c -o out/common.o

imgsender.o : src/server/imgsender.h src/server/imgsender.c
	gcc ${CFLAGS} -c src/server/imgsender.c -o out/imgsender.o

imgreceiver.o : src/client/imgreceiver.h src/client/imgreceiver.c
	gcc ${CFLAGS} -c src/client/imgreceiver.c -o out/imgreceiver.o

client.o: src/client/basicUDPClient.c
	gcc ${CFLAGS} -c src/client/basicUDPClient.c -o out/client.o

UDPserver.o: src/server/UDPserver.h src/server/UDPserver.c
	gcc ${CFLAGS} -c src/server/UDPserver.c -o out/UDPserver.o
server.o: src/server/server.c
	gcc ${CFLAGS} -c src/server/server.c -o out/server.o

main: clean cleanServerOutput client.o server.o common.o UDPserver.o imgsender.o imgreceiver.o
	gcc ${LDFLAGS} out/client.o out/common.o out/imgreceiver.o -o bin/Client
	gcc ${LDFLAGS} out/server.o out/common.o out/UDPserver.o out/imgsender.o -o bin/Server

clean:
	-rm -rf out/*

cleanServerOutput:
	-rm output/copied.jpg
