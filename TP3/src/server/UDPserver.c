#include "UDPserver.h"


int UDPserver(int sock, struct sockaddr_in addr, int port) {

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("socket\n");
    }

    // zero out the structure
    memset((char *) &addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        die("bind\n");
    }

    return sock;
}
