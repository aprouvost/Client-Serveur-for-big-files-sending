#include "UDPserver.h"
#include "imgsender.h"

int main(void){
    struct sockaddr_in addr, addr_other;
    int control_socket;
    int data_socket;
    int recv_len;
    socklen_t slen = sizeof(addr_other);
    char buf[BUFLEN];

    //create UDP socket for control
    if((control_socket = UDPserver(addr, 1234)) == -1){
        die("Error in control socket creation");
    }

    char type[] = "server";
    int val_handshake = handshake(type,control_socket, buf, addr_other);
    if (val_handshake !=0){
      printf("Error, handshake didn't work\n");
      return -1;
    }

    close(control_socket);

    //create UDP socket for data
    if((data_socket = UDPserver(addr, PORTDATA)) == -1){
        die("Error in data socket creation");
    }

    while (1){
        printf("Waiting data\n");
        if ((recv_len = recvfrom(data_socket, buf, BUFLEN, 0, (struct sockaddr *) &addr_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        printf("Data: %s\n" , buf);

        if(strcmp(buf, GETIMGCOMMAND) == 0){
          imageSender (data_socket, "chaton.jpg", addr_other);
        }
        memset(buf, 0, BUFLEN);
    }
    return 0;
}
