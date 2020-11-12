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
#include "client.h"

#define SERVER "127.0.0.1"
#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to send data

void die(char *s)
{
	perror(s);
	exit(1);
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

int receive_image(int socket)
{ // Start function

	printf("Entering receive function \n");

	int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;

	char imagearray[10241],verify = '1';
	FILE *image;
	struct sockaddr_in si_other;
	socklen_t lensock = sizeof(si_other);

	//Find the size of the image
	printf("Wait for img size\n");
	while(stat<0){
	// stat = read(socket, &size, sizeof(int));
	char size_recv[100];
	stat = recvfrom(socket, &size_recv, sizeof(recv_size), 0, (struct sockaddr *) &socket, &lensock);
	sscanf(size_recv, "%d", &recv_size);

	printf("retry receiving \n");
}

	printf("Packet received.\n");
	printf("Packet size: %i\n",size);
	printf("Image size: %i\n",size);
	printf(" \n");

	char buffer[] = "Got it";

	//Send our verification signal
	while(stat<0){
	// stat = write(socket, &buffer, sizeof(int));
	stat = sendto(socket, &buffer, sizeof(int) , 0 , (struct sockaddr *) &socket, lensock);
	}

	printf("Reply sent\n");
	printf(" \n");

	image = fopen("capture2.jpg", "w");

	if( image == NULL) {
	printf("Error has occurred. Image file could not be opened\n");
	return -1; }

	//Loop while we have not received the entire file yet


	int need_exit = 0;
	struct timeval timeout = {10,0};

	fd_set fds;
	int buffer_fd, buffer_out;

	while(recv_size < size) {
	//while(packet_index < 2){

	    FD_ZERO(&fds);
	    FD_SET(socket,&fds);

	    buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

	    if (buffer_fd < 0)
	       printf("error: bad file descriptor set.\n");

	    if (buffer_fd == 0)
	       printf("error: buffer read timeout expired.\n");

	    if (buffer_fd > 0)
	    {
	        do{
	               // read_size = read(socket,imagearray, 10241);
								 read_size = recvfrom(socket, imagearray, 10241, 0, (struct sockaddr *) &socket, &lensock);
	            }while(read_size <0);

	            printf("Packet number received: %i\n",packet_index);
	        printf("Packet size: %i\n",read_size);


	        //Write the currently read data into our image file
	         write_size = fwrite(imagearray,1,read_size, image);
	         printf("Written image size: %i\n",write_size);

	             if(read_size !=write_size) {
	                 printf("error in read write\n");    }


	             //Increment the total number of bytes read
	             recv_size += read_size;
	             packet_index++;
	             printf("Total received image size: %i\n",recv_size);
	             printf(" \n");
	             printf(" \n");
	    }
	}
	return 0;
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
	  socklen_t slen_data = sizeof(si_other);
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

			memset(message_data, 0, BUFLEN);
			printf("Enter message : ");
			gets(message_data, BUFLEN, stdin);

			char img_command[] = "GETIMAGE";
			//remove_spaces(&message_data[0]);
			printf("Send data : %s_\n", message_data);

			if (sendto(s_data, message_data, strlen(message_data) , 0 , (struct sockaddr *) &si_data, slen_data)==-1)
			{
				die("sendto()");
			}

			if(strcmp(message_data, img_command)==0){
				printf("Waiting for img\n");
				int img = receive_image(s_data);
				printf("img receinving value : %d\n", img);
				printf("Img received\n");
			}
		}
	}


	close(s);
	return 0;
}
