#include "rsocket.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT 56046

int main(){
    int r_sockfd;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    int len = sizeof(server);

    r_sockfd = r_socket(AF_INET, SOCK_MRP, 0);
    r_bind(r_sockfd, (const struct sockaddr * )&server, sizeof(server));

    char buffer ;
    while(1){
        r_recvfrom(r_sockfd, (void *)&buffer, 1, 0, (struct sockaddr *)&server, &len);
        printf("%c",buffer);
        fflush(stdout);
        fflush(stdin);
    }

    r_close(r_sockfd);
    return 0;
}