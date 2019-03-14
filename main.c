#include "rsocketcontainer.h"
#include "rsocket.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT 10000

#define MAIN_TESTING



int main(){
    int r_socket_test_fd;
    r_socket_test_fd = r_socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    int addr_len = sizeof(server_addr);

    r_bind(r_socket_test_fd, (const struct sockaddr *) & server_addr, sizeof(server_addr));

    char * buffer = (char *)malloc(sizeof(char) * 100);
    int buffer_len = 100;

    recvfrom(r_socket_test_fd, buffer, buffer_len + 1 , 0, (struct sockaddr *) & server_addr, &addr_len);
    printf("\nMessage Recieved : %s", buffer);

    // printf("\nType message : ");
    // scanf("%s", buffer);
    strcpy(buffer, "THIS IS AWESOME");
    printf("\nSending : %s", buffer);

    r_sendto(r_socket_test_fd, buffer, buffer_len, 0, (struct sockaddr *) & server_addr, addr_len);

    r_close(r_socket_test_fd);

    printf("\n");
    return 0;
}