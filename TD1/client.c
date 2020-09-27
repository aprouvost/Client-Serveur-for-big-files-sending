#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    printf("Starting Client \n");
    int sock = 0;
    //struct sockaddr_in serv_addr;
    //memset((char*)&my_addr, 0, sizeof(serv_addr)) ;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    printf(" \n Valeur du descripteur client : %d \n", sock )

    //serv_addr.sin_family = AF_INET;
    //serv_addr.sin_port = htons(PORT);
    //if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    //{
    //    printf("\n Socket creation error for client \n");
    //    int reuse= 1 ;
    //    setsockopt(descripteur, SOL_SOCKET, SO_REUSEADDR,&reuse, sizeof(reuse)) ;
    //    return -1;
    //}

    //char loc = "127.0.0.1"
    //int adr = inet_aton(AF_INET, loc, &serv_addr.sin_addr);
    //printf("\n Valeur retournée par la fonction inet_aton() : %c", adr);


    return 0;
}
