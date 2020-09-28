#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    printf("Starting Client \n");
    int sock = 0;
    int PORT = 1234;
    struct sockaddr_in serv_addr;
    memset((char*)&serv_addr, 0, sizeof(serv_addr)) ;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    printf(" \n Valeur du descripteur client : %d \n", sock );

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (sock < 0)
    {
        printf("\n Socket creation error for client \n");
        int reuse= 1 ;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,&reuse, sizeof(reuse)) ;
        return -1;
    }

    // char *loc = "127.0.0.1";
    // struct sockaddr_in addr;
    // int val = inet_aton(loc, &addr.sin_addr);
    // char *some_addr = inet_ntoa(addr.sin_addr);
    // printf("\n Valeur retournée par la fonction inet_aton() : %s \n", some_addr);
    // printf("\n Valeur retournée par la fonction inet_aton() : %d \n", val);

    // int conv = htons((long) &addr);
    // printf("val conv : %d \n", conv);

    int conn = connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    printf("connect value : %d \n", conn);
    char *caracter;
    char *received;
    caracter=" Hello from client ";
    int res = write(conn, caracter, strlen(caracter));
    //printf("Message sent to server %d\n", res);
    //int res = read(sock, received, 1024);
    printf("Value %d", res);
    printf("Received value from server : %s\n",received );


    // printf("%d\n", adr.s_addr);



    return 0;
}
