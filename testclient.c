#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/select.h>
#include <sys/wait.h>
#include <ctype.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 10000

void print_sockaddr(struct sockaddr * a){
    int i=0 ;
    for(i = 0 ; i < 14 ; i++){
        printf("\ni = %d, %d", i , (int)a->sa_data[i]);
    }

    printf("\nFAMILY  = %d", a->sa_family);
}

int main(){
    int sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1){
        perror("\nError socket ");
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    int len = sizeof(server);

    print_sockaddr((struct sockaddr *)&server);

    if(connect(sockfd, (const struct sockaddr *)&server, len) < -1 ){
        perror("\nError bind ");
        close(sockfd);
        return -1;
    }

    char * buffer = (char *)malloc(sizeof(char) * 101);
    strcpy(buffer, "hello from the other side");
    sendto(sockfd, buffer, 101, 0, NULL, NULL);
    print_sockaddr((struct sockaddr *)&server);

    int recv_n  = recvfrom(sockfd, buffer, 101, 0, NULL, NULL);
    print_sockaddr((struct sockaddr *)&server);

    buffer[recv_n] = '\0';
    printf("Message recieved : %s\n", buffer + 1 );

    return 0;
}