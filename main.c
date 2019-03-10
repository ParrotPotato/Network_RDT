#include "rsocketcontainer.h"
#include "rsocket.h"
#include <stdio.h>
#include <stdlib.h>

#define PORT 10000

int main(){
    int r_socket_test_fd;
    r_socket_test_fd = r_socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    r_bind(r_socket_test_fd, (const struct sockaddr *) & server_addr, sizeof(server_addr));

    r_close(r_socket_test_fd);
    return 0;
}