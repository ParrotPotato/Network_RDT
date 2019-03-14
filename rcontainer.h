#ifndef HEADER_RCONTAINER
#define HEADER_RCONTAINER

#include <stdlib.h>
#include <stdio.h>

#include "rsocketcontainer.h"


// container for receive buffer (preety self explanatory)
struct def_receive_buffer_container{
    int message_id;
    char * buffer;
};
typedef struct def_receive_buffer_container receive_buffer_container;

// container for unacknowledge messages (preety self explanatory)
struct def_unacknowledged_message_container{
    time_t sending_time;
    int message_id;
    char * buffer;
    int len;
    char * ip;
    unsigned short port;
};
typedef struct def_unacknowledged_message_container unacknowledged_message_container;

// container for received message id's (preety self explanatory)
struct def_receive_message_id_container{
    int message_id;
};
typedef struct def_receive_message_id_container receive_message_id_container;

// thread container
struct def_thread_container{
    int is_close_requested;                                     // check if the close is called on the socket for this thread 
    pthread_t id;
    pthread_attr_t attr;                                        
};
typedef struct def_thread_container thread_container;

// socket container 
struct def_r_socket_container{
    int udp_sockfd;
    Table receive_buffer;                                       // (container : receive_buffer_container            , key : message_id)
    Table unacknowledged_message;                               // (container : unacknowledged_message_container    , key : ??)(nitesh : can be sending_time or message_id)
    Table receive_message_id;                                   // (container : receive_message_id_container        , key : message_id)
    pthread_mutex_t lock;
    pthread_cond_t cond;
    struct sockaddr * addr;                                  // stores the addr for the socket
    int is_usable;
    int counter;
};
typedef struct def_r_socket_container r_socket_container;


extern void print_receive_message_id(receive_message_id_container * ptr);
extern void print_unacknowledged_message(unacknowledged_message_container * ptr);
extern void print_receive_buffer(receive_buffer_container * ptr);



#endif