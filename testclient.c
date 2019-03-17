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
#include <pthread.h>


#define PORT 10000

void print_sockaddr(struct sockaddr * a){
    int i=0 ;
    for(i = 0 ; i < 14 ; i++){
        printf("\ni = %d, %d", i , (int)a->sa_data[i]);
    }

    printf("\nFAMILY  = %d", a->sa_family);
}


int exitstate = 0;

void * running_recv(void * args){
    fd_set rset;
    FD_ZERO(&rset);
    int sockfd = *((int*)args);
    while(exitstate == 0){
        FD_SET(sockfd, &rset);

        int select_return = select(sockfd + 1 , &rset, NULL, NULL, NULL);

        if(select_return == 0){
            printf("Nothing happened\n");
        }
        else if(select_return == -1){
            printf("Something bad happened\n");
        }
        else{
            
            char buffer[100];
            int recv_n = recvfrom(sockfd, buffer, 100, 0, NULL, NULL);
            if(buffer[0] == 'a'){
                printf("Acknowledgement for message_id : %d\n", (int)buffer[1]);
            }
            else{
                printf("Message id : %d\nMessage buffer : %s\n", (int)buffer[0], buffer + 1 );
            }
        }
    }

    return NULL;
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

    if(connect(sockfd, (const struct sockaddr *)&server, len) < -1 ){
        perror("\nError bind \n");
        close(sockfd);
        return -1;
    }

    pthread_t thread_id = 0 ;
    pthread_create(&thread_id, NULL, running_recv, (void *)&sockfd);

    char buffer[100];
    int message_id = 0 ;
    char acknowledgement_buffer[3];

    while(exitstate == 0){
        printf("Enter message ID : ");
        scanf("%d", &message_id);
        fflush(stdin);
        if(message_id == -1){
            printf("Enter message ID : ");
            scanf("%d", &message_id);
            fflush(stdin);

            acknowledgement_buffer[0] = 'a';
            acknowledgement_buffer[1] = (char)message_id;
            acknowledgement_buffer[2] = '\0';
            int send_n = sendto(sockfd, acknowledgement_buffer, 3, 0, &server, len);
            continue;
        }
        scanf("%s", buffer + 1);
        printf("Sending message : %s\n", buffer + 1);
        if(strcmp(buffer + 1,"QUIT") == 0){
            exitstate = 1 ;
        }
        buffer[0] = (char)message_id;
        int send_n = sendto(sockfd, buffer, strlen(buffer + 1) + 2 , 0, &server, len);
    }
    printf("QUITING MAIN\n");
    pthread_join(thread_id, NULL);

    printf("EXIT\n");
    return 0;
}