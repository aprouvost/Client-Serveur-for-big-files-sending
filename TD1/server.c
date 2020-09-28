#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    printf("Starting Server \n");
    int server_fd, new_socket, valread;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf(" \n Valeur du descripteur serveur : %d \n", server_fd );


    struct sockaddr_in address;
    int PORT = 1234;
    memset((char*)&address, 0, sizeof(address)) ;
    //server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    printf(" valeur de INADDR_ANY : %d  \n", INADDR_ANY);
    printf(" valeur de LOOPBACK : %d \n", INADDR_LOOPBACK);

    int reuse= 1 ;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,&reuse, sizeof(reuse)) ;
    if (server_fd  < 0) {
        printf("\n Socket creation error for server \n");
        return -1;
    }

    int connect = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    int max_co = 3;
    int is_listening = listen(server_fd, max_co);

    //----------------------------
    //boucles infinies :
    //
    struct sockaddr_in addr;
    socklen_t var = sizeof(addr);

    for(;;){
      int ack = accept(server_fd, (struct sockaddr *) &address, &var);
      printf("Accepted %d\n", ack );
      char *caracter ;
      char *received;
      caracter = "Hello from server ";
      //int res = write(ack, caracter, strlen(caracter));
      //printf("Message sent to client \n");
      int res = read(ack, received, 1024);
      printf("Value %d", res);
      //printf("Received value from client : %s\n",received );
    }

    //ou

    //while(1)
    //        ;


    //int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen));
    //printf("\n Valeur retournée par la fonction accept() : %c", new_socket);

    return 0;
}
