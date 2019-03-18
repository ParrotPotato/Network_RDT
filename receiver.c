// THE SENDER FOR THE WORD

#include "rsocket.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT 10000

int main(){
    int r_sockfd;
    r_sockfd = r_socket(AF_INET, SOCK_MRP, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int addr_len = sizeof(server);

    if(r_connect(r_sockfd, (const struct sockaddr * )&server, addr_len) < 0){
        perror("Connect");
        exit(-1);
    }
    char * buffer = "TESTING BUFFER";
    //r_sendto(r_sockfd, (void *)buffer, strlen(buffer) + 1, 0, (const struct sockaddr * )&server, addr_len);
    int counter = 0 ;
    while(counter < strlen(buffer)){
        r_sendto(r_sockfd, (void *)&buffer[counter], 1, 0, (const struct sockaddr *)&server, addr_len);
        counter++;
    }

    r_close(r_sockfd);

    return 0;
}
