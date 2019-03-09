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
#include "rsocketcontainer.h"



#define STDERR 2


// assume max message size to be 100
// the assignment is not designed to handle more than 1 connection at  agiven time 

struct def_receive_buffer_container{
    int message_id;
    char buffer[100];
};
typedef struct def_receive_buffer_container receive_buffer_container;

struct def_unacknowledged_message_container{
    int message_id;
    char buffer[100];
    time_t sending_time;
};
typedef struct def_unacknowledged_message_container unacknowledged_message_container;

struct def_receive_message_id_container{
    int message_id;
};
typedef struct def_receive_message_id_container receive_message_id_container;

int is_close_requested = 0 ;


// the thread for handeling all the updating tables 
pthread_t * threadX;
pthread_attr_t attr;
// mutation lock for the table updating ( as they are shared between the API calls and thread X)
pthread_mutex_t * lock;

void * threadX_routine(void * attr){

}


int r_socket(int domain, int type, int protocol){
    // creates a UDP socket with given information
    int udp_socket = 0;
    udp_socket = socket(domain, type, protocol);
    if(udp_socket == -1){
        return -1;
    }

    // iniaializes all the tables and their data

    // creates thread X
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int ret = pthread_create(&threadX, &attr, threadX_routine,NULL);
    if(ret == -1){
        perror("Unable to create Thread X\n");
        close(udp_socket);
        return -1;
    }
    return udp_socket;
}

int r_bind(int sockfd, const struct sockaddr * addr, socklen_t len){
    return bind(sockfd, addr, len);
}


int r_close(int sockfd){
    // close the socket delete all the related resource and buffers.
}