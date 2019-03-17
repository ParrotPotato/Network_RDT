

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
    int r_socket_test_fd;
    r_socket_test_fd = r_socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    int addr_len = sizeof(server_addr);

    // print_sockaddr((struct sockaddr *)&server_addr);

    r_bind(r_socket_test_fd, (const struct sockaddr *) & server_addr, sizeof(server_addr));

    // print_sockaddr((struct sockaddr *)&server_addr);


    char * buffer = (char *)malloc(sizeof(char) * 100);
    int buffer_len = 100;

    r_recvfrom(r_socket_test_fd, buffer, buffer_len + 1 , 0, (struct sockaddr *) & server_addr, &addr_len);
    // recvfrom(r_socket_test_fd, buffer, buffer_len + 1 , 0, NULL, NULL);

    printf("\nMessage Recieved : %s", buffer);

    // print_sockaddr((struct sockaddr *)&server_addr);


    // printf("\nType message : ");
    // scanf("%s", buffer);
    strcpy(buffer, "THIS IS AWESOME");
    printf("\nSending : %s", buffer);

    r_sendto(r_socket_test_fd, buffer, buffer_len, 0, (struct sockaddr *) & server_addr, addr_len);

    strcpy(buffer, "This is the first message from the server side");
    printf("\nSending : %s\n", buffer);

    r_sendto(r_socket_test_fd, buffer, buffer_len, 0, (struct sockaddr *) & server_addr, addr_len);

    strcpy(buffer, "This is the second message from the server side");
    printf("\nSending : %s\n", buffer);

    r_sendto(r_socket_test_fd, buffer, buffer_len, 0, (struct sockaddr *) & server_addr, addr_len);

    strcpy(buffer, "This is the third message from the server side");
    printf("\nSending : %s\n", buffer);

    r_sendto(r_socket_test_fd, buffer, buffer_len, 0, (struct sockaddr *) & server_addr, addr_len);


    printf("\n\nMAIN THREAD SLEEPING FOR 15 SECONDS\n\n");
    sleep(15);

    while(1){
        r_recvfrom(r_socket_test_fd, buffer, buffer_len + 1 , 0, (struct sockaddr *) & server_addr, &addr_len);
        printf("\nMessage Recieved : %s\n", buffer);
        if(strcmp(buffer, "QUIT") == 0)
        {
            break;
        }
    }

    printf("Quitting the MAIN THREAD \n");
    r_close(r_socket_test_fd);

    printf("\n");
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