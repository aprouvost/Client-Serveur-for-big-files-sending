#include "imgreceiver.h"
#include <resolv.h>

int imgReceiver(int chaussette, struct sockaddr_in addr){

  char buffer[BUFLEN];
  int taille;
  socklen_t slen = sizeof(addr);
  char new_file[]="output/copied.jpg";
  int receivedBytes, writtenBytes;

  memset(buffer, 0, BUFLEN);
  recvfrom(chaussette, buffer, BUFLEN,0, (struct sockaddr *)&addr, &slen);
  printf("String size received : %s\n",buffer);
  sscanf(buffer, "%d", &taille);
  printf("size of file : %d\n", taille);

  printf("Sending ACK\n");
  if(taille != 0){
    sendto(chaussette, ACK, sizeof(ACK), 0, (struct sockaddr *)&addr, slen);
  }

  FILE *myFile;
  myFile = fopen(new_file, "wb");

  if(myFile == NULL)
 {
   die("Error when creating file.");
 }

 int sizeCheck = 0;

 printf("Receiving file ...\n");
 if(sizeCheck < taille){
   while (sizeCheck < taille) {
     receivedBytes = recvfrom(chaussette, buffer, BUFLEN, 0, (struct sockaddr *)&addr, &slen);

     // sending ACK to server
     printf("Sending ACK\n");
     sendto(chaussette, ACK, sizeof(ACK), 0, (struct sockaddr *)&addr, slen);

     //writting datas in file
     writtenBytes = fwrite(buffer, 1, receivedBytes, myFile);
     fflush(myFile);
     sizeCheck += writtenBytes;
   }
   
 } else {
   receivedBytes = recvfrom(chaussette, buffer, BUFLEN, 0, (struct sockaddr *)&addr, &slen);
   fwrite(buffer, 1, receivedBytes, myFile);
   fflush(myFile);
 }

 if(taille == sizeCheck){
  printf("Success.");
  } else{
    printf("Fail.");
  }

  fclose(myFile);
  return 0;


}
