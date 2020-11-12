/*
	Simple udp client
*/
#include<stdio.h>	//printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUFLEN 512	//Max length of buffer

void die(char *s)
{
	perror(s);
	exit(1);
}

int main (int argc, char *argv[]) {

  if (argc < 3){
    printf("2 arg needed, <@ip> <port> \n");
    return -1;
  }

  struct sockaddr_in adresse;
  int port= atoi(argv[2]);
  int res = inet_aton(argv[1], &adresse.sin_addr);
  if (res < 0){
    printf("Incompatible ip address");
    exit(EXIT_FAILURE);
  }

	int s, slen=sizeof(adresse);
	char message[BUFLEN];

	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}

	memset((char *) &adresse, 0, sizeof(adresse));
	adresse.sin_family = AF_INET;
	adresse.sin_port = htons(port);

	while(1)
	{
		printf("Enter message : ");
		fgets(message, BUFLEN, stdin);

		//send the message
		if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &adresse, slen)==-1)
		{
			die("sendto()");
		}
	}

	close(s);
	return 0;
}
