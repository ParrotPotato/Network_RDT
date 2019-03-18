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


    r_sockfd = r_socket(AF_INET, SOCK_DGRAM, 0);
    r_connect(r_sockfd, (const struct sockaddr * )&server, sizeof(server));

    char buffer[100];
    scanf("%[^\n]s",buffer);
    printf("READ : %s\n", buffer);

    int counter = 0 ;
    while(counter < strlen(buffer)){
        r_sendto(r_sockfd, (void *)&buffer[counter], 1, 0, (const struct sockaddr *)&server, sizeof(server));
        counter++;
    }

    r_close(r_sockfd);
    return 0;
}