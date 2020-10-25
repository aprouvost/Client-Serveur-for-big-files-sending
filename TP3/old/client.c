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

#define SERVER "127.0.0.1"
#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to send data

void die(char *s)
{
	perror(s);
	exit(1);
}

int main(void)
{
	struct sockaddr_in si_other;
	int s, recv_len;
  socklen_t slen=sizeof(si_other);
	char message[BUFLEN];
  char buf[BUFLEN];

	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);

	if (inet_aton(SERVER , &si_other.sin_addr) == 0)
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

  // Connexion
  printf("Sending SYN\n");
  char *message_sin = "SYN";
  if (sendto(s, message_sin, strlen(message_sin) , 0 , (struct sockaddr *) &si_other, slen)==-1)
  {
    die("sendto()");
  }

  memset(buf, 0, sizeof(buf));
  printf("Data: %s\n" , buf);
  printf("Waiting SYN-ACK\n");

  if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
  {
    die("recvfrom()");
  }
  printf("Data: %s\n" , buf);
  char syn_ack_msg[] = "SYN-ACK";
  if(strcmp(buf, syn_ack_msg) == 0){
    printf("SYN-ACK received\n");

    char *ack = "ACK";
    printf("Sending ACK \n");
    if (sendto(s, ack, strlen(ack) , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
      die("sendto()");
    }
	}


	//Receiving the new port number from server to start data stream on new socket
  memset(buf, 0, sizeof(buf));
	char port_data;
	printf("Waiting for port new number\n");
	if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
	{
		die("recvfrom()");
	}
	else{
		close(s);
		int port_value;
		sscanf(buf, "%d", &port_value);

		printf("%d\n", port_value );
		struct sockaddr_in si_data;
		int s_data, recv_len_data;
	  socklen_t slen_data=sizeof(si_other);
		char message_data[BUFLEN];
	  char buf_data[BUFLEN];

		//socket s_data is used to send the data stream, on port port_data
		if ( (s_data=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		{
			die("socket");
		}

		memset((char *) &si_data, 0, sizeof(si_data));
		si_data.sin_family = AF_INET;
		si_data.sin_port = htons(port_value);

		if (inet_aton(SERVER , &si_data.sin_addr) == 0)
		{
			fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}

		while(1)
		{
			printf("Enter message : ");
			fgets(message_data, BUFLEN, stdin);

			//send the message
			if (sendto(s_data, message_data, strlen(message_data) , 0 , (struct sockaddr *) &si_data, slen_data)==-1)
			{
				die("sendto()");
			}
		}
	}


	close(s);
	return 0;
}
