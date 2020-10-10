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

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

int main (int argc, char *argv[]) {

  if (argc < 2){
    printf("One arg needed, <port> \n");
    return -1;
  }

  int port = atoi(argv[1]), valid = 1, recv_len, serv_desc_TCP, serv_desc_UDP;
  char buffer[RCVSIZE];
  struct sockaddr_in TCPaddr, UDPaddr, client;
  socklen_t alen = sizeof(client);

  // Create socket

  // TCP

  if((serv_desc_TCP = socket(AF_INET, SOCK_STREAM, 0))  == -1){
    perror("Cannot create TCP socket \n");
    exit(EXIT_FAILURE);
  }

  setsockopt(serv_desc_TCP, SOL_SOCKET, SO_REUSEADDR, &valid, sizeof(int));

  TCPaddr.sin_family = AF_INET;
  TCPaddr.sin_port = htons(port);
  TCPaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(serv_desc_TCP, (struct sockaddr*) &TCPaddr, sizeof(TCPaddr)) == -1){
    perror("TCP bind failed \n");
    close(serv_desc_TCP);
    exit(EXIT_FAILURE);
  }

  if(listen(serv_desc_TCP, 0 ) < 0){
    printf("TCP listen failed\n");
    exit(EXIT_FAILURE);
  }

  printf("Listen done \n");

  //UDP

  if((serv_desc_UDP = socket(AF_INET, SOCK_DGRAM, 0))  == -1){
    perror("Cannot create UDP socker \n");
    exit(EXIT_FAILURE);
  }

  memset((char *) &UDPaddr, 0, sizeof(UDPaddr));

  UDPaddr.sin_family = AF_INET;
  UDPaddr.sin_port = htons(4545);
  UDPaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(serv_desc_UDP, (struct sockaddr*)&UDPaddr, sizeof(UDPaddr)) == -1){
    perror("UDP bind failed \n");
    exit(EXIT_FAILURE);
  }

  // Select initialize
  fd_set sock_set;
  FD_ZERO(&sock_set);
  int maxfdp1 = max(serv_desc_TCP, serv_desc_UDP) + 1;

  while (1) {

    printf("Waiting for data \n");

    // Select set up
    FD_SET(serv_desc_TCP, &sock_set);
    FD_SET(serv_desc_UDP, &sock_set);

    select(maxfdp1, &sock_set, NULL, NULL, NULL);

    if(FD_ISSET(serv_desc_TCP, &sock_set)){ // TCP
      printf("C'est du TCP qui passe\n ");
      // socklen_t tcplen = sizeof(TCPaddr);
      int client_desc = accept(serv_desc_TCP, (struct sockaddr*)&client, &alen);
      printf("Value of accept is:%d\n", client_desc);

      int pid = fork();
      if (pid==0){
        close(serv_desc_TCP);
        bzero(buffer, sizeof(buffer));
        printf("Message TCP from client\n" );
        read(client_desc, buffer, RCVSIZE);
        printf("data: %s\n", buffer);
        close(client_desc);
        exit(0);
      }
      if (pid<0){
        printf(" Error for fork function ");
        exit(EXIT_FAILURE);
      }
    }


    if(FD_ISSET(serv_desc_UDP, &sock_set)){ // UDP
      printf("C'est de l'UDP qui passe \n" );
      // socklen_t udplen = sizeof(UDPaddr);
      bzero(buffer, sizeof(buffer));
      printf("Receiving UDP message from client : \n" );
      if((recv_len = recvfrom(serv_desc_UDP, buffer, RCVSIZE, 0, (struct sockaddr *) &client, &alen)) == -1){
        perror("recvfrom failed \n");
        exit(EXIT_FAILURE);
      }

      printf("DATA: %s\n", buffer);
    }
  }
  close(serv_desc_TCP);
  //close(serv_desc_UDP);
printf("Ending programm\n" );
return 0;
}
