#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>


#define RCVSIZE 1024
#define MAXLINE 1024

void die(char *s)
{
	perror(s);
	exit(1);
}

int main (int argc, char *argv[]) {

  if (argc < 2){
    printf("One arg needed, <port> \n");
    return -1;
  }

  struct sockaddr_in adresse, client, UDPaddr, cliaddr;
  // int port= 5001;
  int port= atoi(argv[1]);

  int valid= 1;
  socklen_t alen= sizeof(client);
  char buffer[RCVSIZE];

  //create socket
  int server_desc = socket(AF_INET, SOCK_STREAM, 0);

  int server_desc_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  //handle error
  if (server_desc < 0 || server_desc_udp < 0) {
    perror("Cannot create socket\n");
    return -1;
  }

  setsockopt(server_desc, SOL_SOCKET, SO_REUSEADDR, &valid, sizeof(int));

  adresse.sin_family= AF_INET;
  adresse.sin_port= htons(port);
  adresse.sin_addr.s_addr= htonl(INADDR_ANY);

  //initialize socket
  if (bind(server_desc, (struct sockaddr*) &adresse, sizeof(adresse)) == -1) {
    perror("Bind failed\n");
    close(server_desc);
    return -1;
  }


  memset((char *) &UDPaddr, 0, sizeof(UDPaddr));

  UDPaddr.sin_family = AF_INET;
  UDPaddr.sin_port = htons(4567);
  UDPaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(server_desc_udp, (struct sockaddr*) &UDPaddr, sizeof(UDPaddr)) == -1){
    perror("Bind failed \n");
    close(server_desc_udp);
    return -1;
  }


  //listen to incoming clients
  if (listen(server_desc, 0) < 0) {
    printf("Listen failed tcp\n");
    return -1;
  }

  printf("Listen done\n");

  fd_set sock_set;
  //int timeout = 5;
  //truct timeval select_timeout;
  //select_timeout.tv_sec = timeout;
  //select_timeout.tv_usec = 0;

  FD_ZERO(&sock_set);

  while (1) {


    FD_SET(server_desc_udp, &sock_set);
    FD_SET(server_desc, &sock_set);


    int max_input = 4;
    select(max_input, &sock_set, NULL, NULL, NULL);
    if (FD_ISSET(server_desc_udp, &sock_set)){ // si c'est de l'UDP
      printf("C'est de l'UDP qui passe \n" );

      //lecture du message
      int slen = sizeof(cliaddr) , recv_len;
      if ((recv_len = recvfrom(server_desc_udp, buffer, RCVSIZE, 0, (struct sockaddr *) &cliaddr, &aslen)) == -1)
      		{
      			die("recvfrom()");
      		}

      		//print details of the client/peer and the data received
      		printf("Received packet from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
      		printf("Data: %s\n" , buffer);

    }


    if (FD_ISSET(server_desc, &sock_set)){ // si c'est de l'UDP
      printf("C'est du TCP qui passe \n" );

      int client_desc = accept(server_desc, (struct sockaddr*)&client, &alen);
      printf("Value of accept is:%d\n", client_desc);

      int pid = fork();

      if (pid<0){
        printf(" Error for fork function ");
        exit(EXIT_FAILURE);
      } else if (pid == 0){
        printf("Socket value after accepted: %d \n", client_desc);
        close(server_desc);
        int msgSize = read(client_desc,buffer,RCVSIZE);
        while (msgSize > 0) {
          write(client_desc,buffer,msgSize);
          printf("%s",buffer);
          memset(buffer,0,RCVSIZE);
          msgSize = read(client_desc,buffer,RCVSIZE);
        }
        printf("Ending process");
        exit(EXIT_SUCCESS);
      }else if (pid > 0){
        close(client_desc);
      }
    }
    //printf("Accepting\n");
    //printf("Socket value on Server side: %d \n", server_desc);
  }

//close(server_desc);
return 0;
}
