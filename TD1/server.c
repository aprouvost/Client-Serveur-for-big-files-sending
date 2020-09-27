#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    printf("Starting Server \n");
    int server_fd, new_socket, valread;
    server_fd = socket(AF_INET, SOCK_STREAM, 0)
    printf(" \n Valeur du descripteur serveur : %d \n", server_fd )


    //struct sockaddr_in address;
    //memset((char*)&my_addr, 0, sizeof(address)) ;
    //server_fd = socket(AF_INET, SOCK_STREAM, 0);
    //address.sin_family = AF_INET;
    //address.sin_addr.s_addr = INADDR_ANY;
    //address.sin_port = htons( PORT );
    //server_fd = socket(AF_INET, SOCK_STREAM, 0))

    //if ((server_fd  < 0)
    //{
    //    printf("\n Socket creation error for server \n");
    //    int reuse= 1 ;
    //    setsockopt(descripteur, SOL_SOCKET, SO_REUSEADDR,&reuse, sizeof(reuse)) ;
    //    return -1;
    //}

    //int connect = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    //int max_co = 3;
    //int is_listening = listen(server_fd, max_co);

    //----------------------------
    //boucles infinies :

    //for(;;)
    //        ;

    //ou

    //while(1)
    //        ;


    //int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen));
    //printf("\n Valeur retournée par la fonction accept() : %c", new_socket);

    return 0;
}