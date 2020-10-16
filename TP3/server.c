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
#define PORTDATA 6666

void die(char *s)
{
	perror(s);
	exit(1);
}

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}


int main(void)
{
	struct sockaddr_in si_me, si_other, si_me_data;

	int s , recv_len, s_data;
  socklen_t slen = sizeof(si_other);
	char buf[BUFLEN];
  //char message[BUFLEN];



	//create a UDP socket for control
	//-------------------------------------------------------------
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket flux control");
	}

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		die("bind flux control");
	}


	//create a UDP socket for data
	//-------------------------------------------------------------
	if ((s_data=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket flux data");
	}

	// zero out the structure
	memset((char *) &si_me_data, 0, sizeof(si_me_data));

	si_me_data.sin_family = AF_INET;
	si_me_data.sin_port = htons(PORTDATA);
	si_me_data.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if( bind(s_data , (struct sockaddr*)&si_me_data, sizeof(si_me_data)) == -1)
	{
		die("bind flux data");
	}

	fd_set sock_set;
  FD_ZERO(&sock_set);
  int maxfdp1 = max(s, s_data) + 1;

	while (1) {

			FD_SET(s, &sock_set);
			FD_SET(s_data, &sock_set);

			select(maxfdp1, &sock_set, NULL, NULL, NULL);

			//----------------------------------------------------------------
		  // Connexion on the first port
		if(FD_ISSET(s, &sock_set)){
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
		    char ack_msg[] = "ACK";
		    if(strcmp(buf, ack_msg)==0){
		      printf("ACK received\n" );

					//senfing to client the new port number for the data
					memset(buf, 0, BUFLEN);
					char port_value[32] = "";
					// port_value= itoa(PORTDATA, 10);
					sprintf(port_value, "%d", PORTDATA);
					printf("Sending port value : %s\n", port_value);
					if (sendto(s, port_value, strlen(port_value) , 0 , (struct sockaddr *) &si_other, slen)==-1)
					{
						die("sendto()");
					}
		    }


		}

		if(FD_ISSET(s_data, &sock_set)){
			//keep listening for data
				fflush(stdout);

				//try to receive some data, this is a blocking call
				if ((recv_len = recvfrom(s_data, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
				{
					die("recvfrom()");
				}

				//print details of the client/peer and the data received
				printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
				printf("Data: %s\n" , buf);
		}
	}
	close(s);
	return 0;
}
