#include "imgreceiver.h"

tototototototo
int main(void){
    struct sockaddr_in si_other;
    int s; // socket de contrôle
    int s_data; //socket de flux de data
    socklen_t slen=sizeof(si_other);
    char message[BUFLEN];
//    char buf[BUFLEN];

    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORTCONTROL);

    if (inet_aton(SERVER , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }


    //handshake coté client

    char *type = "client";
    int port_data = handshake(type, s, message, si_other );
    close(s);

    printf("port data: %d\n", port_data);

    if ( (s_data=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    si_other.sin_port = htons(port_data);

    while(1) {

        //memset(message, 0, BUFLEN);
        printf("Enter message : ");
        gets(message, BUFLEN, stdin);
        printf("Send data : %s_\n", message);

        if (sendto(s_data, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == -1) {
            die("sendto()");
        }

        if(strcmp(message, GETIMGCOMMAND) == 0){
          imgReceiver(s_data, si_other);
        }
        //close(s_data);
    }

    close(s_data);
    return 0;
}
