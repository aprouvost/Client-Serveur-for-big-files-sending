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
  char type_client[]="client";
  char type_server[]="server";

  //si l'entité est un client, il fait le handshake sur socket d'écoute
  //et reçoit
  //la valeur du nouveaux port pour socket de data qu'il créé

  if(strcmp(type, type_client) == 0){
    socklen_t slen=sizeof(si_other);
    int recv_len;

    // Connexion
    printf("Sending SYN\n");
    if (sendto(s, SYN, strlen(SYN) , 0 , (struct sockaddr *) &si_other, slen)==-1)
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
    if(strcmp(buf, SYNACK) == 0){
      printf("SYN-ACK received\n");

      printf("Sending ACK \n");
      if (sendto(s, ACK, strlen(ACK) , 0 , (struct sockaddr *) &si_other, slen)==-1)
      {
        die("sendto()");
      }
    }
    //Receiving the new port number from server to start data stream on new socket
    int port_value = -1;
    memset(buf, 0, BUFLEN);
    printf("Waiting for port new number\n");
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
      die("recvfrom()");
    }

    sscanf(buf, "%d", &port_value);
    printf("port new value received is %d\n", port_value );

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
    if(strcmp(buf, SYN) == 0){
      // Send SYN-ACK
      printf("Sending SYN-ACK : %s\n", SYNACK);
      if (sendto(s, SYNACK, strlen(SYNACK) , 0 , (struct sockaddr *) &si_other, slen)==-1)
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
      if(strcmp(buf, ACK)==0){
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
