#include "imgsender.h"
#include <netdb.h>



int imageSender (int chaussette,  char *filePath, struct sockaddr_in addr_serv){
  FILE *picture;
  char buffer[BUFLEN];
  socklen_t slen = sizeof(addr_serv);
  char path[] = "input/";

  strcat(path,filePath);
  printf("file : %s\n", path );
  picture = fopen(path, "r");
  if (picture != 0) {

    printf( "DEBUG---- Fichier ouvert avec succès !\n" );
    //besoin de la taille du fichier
    int taille;
    fseek(picture,0, SEEK_END);
    taille = ftell(picture);
    rewind (picture);
    int sizeCheck = 0;
    int readBytes, sentBytes;

    // Send file size
    memset(buffer, 0, BUFLEN);
    sprintf(buffer, "%d", taille);
    printf("Sending size\n");
    sendto(chaussette, buffer, sizeof(buffer), 0, (struct sockaddr *) &addr_serv, slen);
    printf("Size send : %s\n",buffer);

    // ACK size
    printf("Waitting for ACK\n");
    recvfrom(chaussette,buffer,BUFLEN,0,  (struct sockaddr *)&addr_serv, &slen);
    if(strcmp(buffer, ACK) == 0){
      printf("Sending file ...\n");
      if(taille > BUFLEN){
        while (sizeCheck < taille) {
          readBytes = fread(buffer, 1, 256,picture);
          sentBytes = sendto(chaussette, buffer, readBytes, 0, (struct sockaddr *) &addr_serv, slen);
          sizeCheck += sentBytes;
        }
      } else{
        readBytes = fread(buffer,1,256,picture);
        sentBytes = sendto(chaussette, buffer, readBytes, 0, (struct sockaddr *) &addr_serv, slen);
      }

      if(taille == sizeCheck){
      printf("Success.\n");
    } else{
      printf("Fail.\n");
    }

    fclose(picture);
    return 0;
    }
  }
}
