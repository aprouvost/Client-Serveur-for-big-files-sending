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
#include "server.h"

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

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

int send_image(int socket){

	FILE *picture;
	int size, read_size, stat, packet_index;
	char send_buffer[10240], read_buffer[256];
	packet_index = 1;
	struct sockaddr_in si_other;
	socklen_t slen = sizeof(si_other);


	picture = fopen("chaton.jpg", "r");
	printf("Getting Picture Size\n");

	if(picture == NULL) {
			 printf("Error Opening Image File \n"); }

	fseek(picture, 0, SEEK_END);
	size = ftell(picture);
	fseek(picture, 0, SEEK_SET);
	printf("Total Picture size: %i\n",size);

	//Send Picture Size
	printf("Sending Picture Size\n");
	// write(socket, (void *)&size, sizeof(int));
	printf("Size value sent : %d\n", size );
	//int message =ChartoInt(size);
	char taille[15];
	sprintf(taille, "%d", size);
	int t = sendto(socket,taille,sizeof(taille),0,(struct sockaddr *)&socket, slen);
	//sendto(socket,(void *)size,  sizeof(int) , 0 , (struct sockaddr *) &socket, slen);

	//Send Picture as Byte Array
	printf("Sending Picture as Byte Array\n");

	//stat = -1;
	while (stat < 0){ //Read while we get errors that are due to signals.
		 // stat=read(socket, &read_buffer , 255);
		 stat = recvfrom(socket, &read_buffer, 255, 0, (struct sockaddr *) &socket, &slen);
		 printf("Bytes read: %i\n",stat);
	}

	printf("Received data in socket\n");
	printf("Socket data: %s\n", read_buffer);

	while(!feof(picture)) {
	//while(packet_index = 1){
		 //Read from the file into our send buffer
		 read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

		 //Send data through our socket
		 //stat = -1;
		 while (stat < 0){
			 // stat = write(socket, send_buffer, read_size);
			 stat = sendto(socket, &send_buffer, read_size , 0 , (struct sockaddr *) &socket, slen);
		 }

		 printf("Packet Number: %i\n",packet_index);
		 printf("Packet Size Sent: %i\n",read_size);
		 printf(" \n");
		 printf(" \n");


		 packet_index++;

		 //Zero out our send buffer
		 bzero(send_buffer, sizeof(send_buffer));
		}
		return 0;
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
		die("socket flux control\n");
	}

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		die("bind flux control \n");
	}


	//create a UDP socket for data
	//-------------------------------------------------------------
	if ((s_data=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket flux data \n");
	}

	// zero out the structure
	memset((char *) &si_me_data, 0, sizeof(si_me_data));

	si_me_data.sin_family = AF_INET;
	si_me_data.sin_port = htons(PORTDATA);
	si_me_data.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if( bind(s_data , (struct sockaddr*)&si_me_data, sizeof(si_me_data)) == -1)
	{
		die("bind flux data \n");
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

		//----------------------------------------------------------------
	  // Connexion on the second port
		if(FD_ISSET(s_data, &sock_set)){
			//keep listening for data
				fflush(stdout);

				//try to receive some data, this is a blocking call
				memset(buf, 0, BUFLEN);
				if ((recv_len = recvfrom(s_data, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
				{
					die("recvfrom()");
				}

				char img_command[] = "GETIMAGE";
				printf("Command  : %s_\n", img_command);
				printf("Buffer  : %s_\n", buf);
			//	remove_spaces(&buf);
				if(strcmp(buf, img_command)==0){
					printf("Sending img\n");
					int img = send_image(s_data);
					printf("img sending value : %d\n", img);
				} else {
				//print details of the client/peer and the data received
				printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
				printf("Data: %s\n" , buf);
			}
		}
	}

	close(s);
	return 0;
}
