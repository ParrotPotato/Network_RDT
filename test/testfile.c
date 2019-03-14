#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <termios.h>

#define PORT 10000


int main(int argc, char **argv)
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server;

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);


    char * ptr = inet_ntoa(server.sin_addr);
    char * ptr2 = inet_ntoa(server.sin_addr);

    printf("Pointer to first inet_ntoa call : %p\n", (void *)ptr);
    printf("Poitner to second inet_ntoa call : %p\n", (void *)ptr2);


    return 0;
}