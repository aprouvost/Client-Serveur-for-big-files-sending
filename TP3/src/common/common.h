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


void die(char *s);
int max(int x, int y);
