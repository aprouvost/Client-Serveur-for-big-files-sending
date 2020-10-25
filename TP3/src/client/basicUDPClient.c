#include "../common/common.h"

#define PORT 1234
#define SERVER_ADDR "127.0.0.1"


int main(void){
    struct sockaddr_in si_other;
    int s;
    socklen_t slen=sizeof(si_other);
    char message[BUFLEN];
//    char buf[BUFLEN];

    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (inet_aton(SERVER_ADDR , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    while(1) {

        memset(message, 0, BUFLEN);
        printf("Enter message : ");
        gets(message, BUFLEN, stdin);
        printf("Send data : %s_\n", message);

        if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == -1) {
            die("sendto()");
        }
    }

    close(s);
    return 0;
}