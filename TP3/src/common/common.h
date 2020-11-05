#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUFLEN 512	//Max length of buffer
#define SYN "SYN"
#define SYNACK "SYNACK"
#define ACK "ACK"
#define SERVER "127.0.0.1"
#define PORTDATA 6666


void die(char *s);
int max(int x, int y);

int handshake (char *type, int s, char buf[BUFLEN],  struct sockaddr_in si_other );
