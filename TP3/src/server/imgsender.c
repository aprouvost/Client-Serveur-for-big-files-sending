#include "imgsender.h"
#include <netdb.h>
#include <sys/time.h>



int imageSender (int chaussette,  char *filePath, struct sockaddr_in addr_serv){
  FILE *picture;
  char buffer[BUFLEN];
  socklen_t slen = sizeof(addr_serv);
  char path[] = "input/";
  struct timeval current_time;
  struct timeval time_checked;


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
      //si doit envoyer avec plusieurs paquets
      if(taille > BUFLEN){
        while (sizeCheck < taille) {
          readBytes = fread(buffer, 1, 256,picture);
          sentBytes = sendto(chaussette, buffer, readBytes, 0, (struct sockaddr *) &addr_serv, slen);

          int received = -1;
          gettimeofday(&time_checked, NULL);
          long double checked = time_checked.tv_sec;
          gettimeofday(&current_time, NULL);
          long double current = current_time.tv_sec;

          double expired = 2.0;
          long double diff = current_time- checked
          //attends ACK en réception avant passer au Byte suivant
          
          while ( diff < expired ){
            recvfrom(chaussette,buffer,BUFLEN,0,  (struct sockaddr *)&addr_serv, &slen);
              if(strcmp(buffer, ACK) == 0){
                diff = expired+ 100;
                //on peut passer au Byte suivant
                sizeCheck += sentBytes;
              }
              else{
                // on est dans les temps mais on n'a pas reçu d'ACK, on a reçu autre chose
                printf("Expected receiving  ACK, received : %s\n", buffer );
              }
          }

          // si on arrive ici sans reception d'ACK ( timeout) on renvoie le meme paquet
          // car on n'aura pas passé la ligne   sizeCheck += sentBytes;
          // donc sizeCheck sera toujours la même

          printf("TIMEOUT, retransmitting \n");

        }

      // si peut tout envoyer en 1 paquet
      } else{
        readBytes = fread(buffer,1,256,picture);
        sentBytes = sendto(chaussette, buffer, readBytes, 0, (struct sockaddr *) &addr_serv, slen);
      }

      // si tout a été envoyé
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

  //   //malloc the buffer big enough for image data
  //
  //   buffer = (char*) malloc (taille);
  //   //read image data into buffer
  //   fread (buffer,1,taille,picture);
  //   int rc = sendto(chaussette, buffer, sizeof(buffer)+1, 0, (struct sockaddr *) &addr_serv, slen);
  //   clearBuf(buffer);
  //
  //   if(rc<0) {
  //     printf("cannot send data \n");
  //     close(chaussette);
  //     exit(1);
  //   }
  //   return(0);
  //
  // }else{
  //   printf("Erreur lors de l'ouverture du fichier\n" );
  //   exit(1);
  // }
  // return(-1);
