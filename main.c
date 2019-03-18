

#define PORT 10000

// #define MAIN_TESTING
#define MAIN_TESTING

#ifdef MAIN_TESTING

#include "rsocket.h"
#include <stdio.h>
#include <stdlib.h>

void print_sockaddr(struct sockaddr * a){
    int i=0 ;
    for(i = 0 ; i < 14 ; i++){
        printf("\ni = %d, %d", i , (int)a->sa_data[i]);
    }

    printf("\nFAMILY  = %d", a->sa_family);
}


int main(){
    int r_sockfd;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    int len = sizeof(server);

    r_sockfd = r_socket(AF_INET, SOCK_DGRAM, 0);
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

#endif

#ifdef TREE_TESTING

#include <stdio.h>
#include <stdlib.h>
#include "rlistcontainer.h"

int main(){
    // printf("Hello world\n");
    Node * root = NULL;
    // printf("Creating somethings 1\n");
    root = insert_node(root, 10, NULL);
    // printf("Creating somethings 2\n");
    root = insert_node(root, 15, NULL);
    // printf("Creating somethings 3\n");
    root = insert_node(root, 3, NULL);
    // printf("Creating somethings 4\n");
    root = insert_node(root, 8, NULL);
    // printf("Creating somethings 5\n");

    print_list(root);

    root = delete_node(root, 10);

    printf("\n");

    print_list(root);

    return 0;
}

#endif