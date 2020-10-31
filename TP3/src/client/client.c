#include "../common/common.h"
#include "UDPclient.h"

#define PORT 1234
#define SERVER_ADDR "127.0.0.1"


int main(void){
    struct sockaddr_in si_other;
    int s_control; // socket de contrôle
    int s_data; //socket de flux de data
    socklen_t slen=sizeof(si_other);
    char message[BUFLEN];
//    char buf[BUFLEN];

    if((s_control = UDPclient(si_other, (char *)SERVER_ADDR, PORT)) == -1){
        die("Error in client control socket");
    }

    printf("port : %d \n", si_other.sin_port);
    printf("addr : %s \n", inet_ntoa(si_other.sin_addr));



    //handshake coté client

    char type[] = "client";
    int port_data = handshake(type, s_control, message, si_other);
    close(s_control);

    if((s_data = UDPclient(si_other, (char *)SERVER_ADDR, port_data)) == -1){
        die("Error in client data socket");
    }

    while(1) {

        memset(message, 0, BUFLEN);
        printf("Enter message : ");
        gets(message);
        printf("Send data : %s_\n", message);

        if (sendto(s_data, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == -1) {
            die("sendto()");
        }
    }

    close(s_data);
    return 0;
}
