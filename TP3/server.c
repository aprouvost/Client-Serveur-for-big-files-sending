/*
	Simple udp server
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
#define PORT 8888	//The port on which to listen for incoming data

void die(char *s)
{
	perror(s);
	exit(1);
}

int main(void)
{
	struct sockaddr_in si_me, si_other;

	int s , recv_len;
  socklen_t slen = sizeof(si_other);
	char buf[BUFLEN];
  //char message[BUFLEN];

	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		die("bind");
	}

  // Connexion
  printf("Waiting for SYN\n");
  if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
  {
    die("recvfrom()");
  }
	printf("SYN received\n");
	printf("Data: %s\n" , buf);
  char syn_msg[] = "SYN";
	if(strcmp(buf, syn_msg) == 0){
    // Send SYN-ACK
    char syn_ack_msg[] = "SYN-ACK";
    printf("Sending SYN-ACK : %s\n", syn_ack_msg);
    if (sendto(s, syn_ack_msg, strlen(syn_ack_msg) , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
      die("sendto()");
    }
  } else {
		printf("No SYN \n" );
	}

    memset(buf, 0, BUFLEN);
    printf("Waiting for ACK\n");
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
      die("recvfrom()");
    }
    char ack_msg[3] = "ACK";
    if(strcmp(buf, ack_msg)==0){
      printf("ACK received\n" );
    }


	//keep listening for data
	while(1)
	{
		printf("Waiting for data...");
		fflush(stdout);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
		{
			die("recvfrom()");
		}

		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n" , buf);
	}

	close(s);
	return 0;
}
