#include "UDPserver.h"


int main(void){
    struct sockaddr_in addr, addr_other;
    int control_socket = -1;
    int recv_len;
    socklen_t slen = sizeof(addr_other);
    char buf[BUFLEN];

    //create UDP socket
    if((control_socket = UDPserver(control_socket, addr, 1234)) == -1){
        die("Error in control socket creation");
    }


    while (1){
        if ((recv_len = recvfrom(control_socket, buf, BUFLEN, 0, (struct sockaddr *) &addr_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        printf("Data: %s\n" , buf);
        memset(buf, 0, BUFLEN);
    }
}
