#include "UDPserver.h"


int main(void){
    struct sockaddr_in addr, addr_other;
    int control_socket;
    int recv_len;
    socklen_t slen = sizeof(addr_other);
    char buf[BUFLEN];

    //create UDP socket
    if((control_socket = UDPserver(addr, 1234)) == -1){
        die("Error in control socket creation");
    }

    char type[] = "server";
    int val_handshake = handshake(type,control_socket, buf, addr_other);
    if (val_handshake !=0){
      printf("Error, handshake didn't work\n");
      return -1;
    }
    while (1){
        if ((recv_len = recvfrom(control_socket, buf, BUFLEN, 0, (struct sockaddr *) &addr_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        printf("Data: %s\n" , buf);
        memset(buf, 0, BUFLEN);
    }
    return 0;
}
