#include "common.h"
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


int handshake (char *type, int s, char buf[BUFLEN], struct  sockaddr_in si_other ){
  char type_client[] = "client";
  char type_server[] = "server";

  //si l'entité est un client, il fait le handshake sur socket d'écoute
  //et reçoit
  //la valeur du nouveaux port pour socket de data qu'il créé

  if(strcmp(type, type_client) == 0){
    socklen_t slen=sizeof(si_other);
    int recv_len;

    // Connexion
    printf("Sending SYN\n");
    char *message_sin = "SYN";
    if (sendto(s, message_sin, strlen(message_sin) , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
      die("sendto()");
    }

    memset(buf, 0, BUFLEN);
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
    int port_value;
    memset(buf, 0, BUFLEN);
    printf("Waiting for port new number\n");
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
      die("recvfrom()");
    }
    else{
      close(s);
      sscanf(buf, "%d", &port_value);

      printf("port new value received is %d\n", port_value );
      struct sockaddr_in si_data;
      int s_data;

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

    }
    //retourne le nouveau port de création de la socket de data
    return port_value;
  }


  //si c'est un serveur
  if(strcmp(type, type_server) == 0){

    //----------------------------------------------------------------
    //fait passer msg de controle sur la socket d'écoute
    //pas besoin de select pour le moment
    socklen_t slen=sizeof(si_other);
    printf("Waiting for SYN\n");
    int recv_len;
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
      return 0;

  }
  //sinon, inconnu
  else{
    printf("WARNING Unknown case !\n" );
    exit(EXIT_FAILURE);
    return(-1);
  }
}
