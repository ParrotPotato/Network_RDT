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

#define debug_flag 0
#define debug_log(udp_socket, ...) debug_flag == 1 ? printf(__VA_ARGS__) : 0;\
                                   debug_flag == 1 ? printf("(socket : %d)",udp_socket) : 0;

#define debug_comment //

#define FLUSH() debug_comment fflush(stdin);fflush(stdout);fflush(stderr)


// container for receive buffer (preety self explanatory)
struct def_receive_buffer_container{
    int message_id;
    char buffer[100];
};
typedef struct def_receive_buffer_container receive_buffer_container;

// container for unacknowledge messages (preety self explanatory)
struct def_unacknowledged_message_container{
    time_t sending_time;
    int message_id;
    char buffer[100];
};
typedef struct def_unacknowledged_message_container unacknowledged_message_container;

// container for received message id's (preety self explanatory)
struct def_receive_message_id_container{
    int message_id;
};
typedef struct def_receive_message_id_container receive_message_id_container;

// socket container 
struct def_r_socket_container{
    int udp_sockfd;
    Table receive_buffer;                                       // (container : receive_buffer_container            , key : message_id)
    Table unacknowledged_message;                               // (container : unacknowledged_message_container    , key : ??)(nitesh : can be sending_time or message_id)
    Table receive_message_id;                                   // (container : receive_message_id_container        , key : message_id)
};
typedef struct def_r_socket_container r_socket_container;

// thread container
struct def_thread_container{
    int is_close_requested;                                     // check if the close is called on the socket for this thread 
    pthread_t id;                                               // TODO : I dont believe even this is required, just check
    pthread_attr_t attr;                                        
};
typedef struct def_thread_container thread_container;

// Table of socket container and thread container 
Table socket_table = NULL;                                      // container : r_socket_container, key : udp_socket
Table thread_table = NULL;                                      // container : thread_container, key : udp_socket

// locks for these table manipulator
pthread_mutex_t socket_table_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_table_lock = PTHREAD_MUTEX_INITIALIZER;

// conditional locks for the variables 
pthread_cond_t thread_table_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t socket_table_cond = PTHREAD_COND_INITIALIZER;


// no fucking clue what I worte here 5 hrs ago
void * X_routine(void * attr){
    
    r_socket_container * socket_container = (r_socket_container *) attr;
    int udp_sockfd = socket_container->udp_sockfd;
    int exit_status = 0 ;
    debug_log(udp_sockfd,"\nThread X running ");
    thread_container * thread_ptr = (thread_container *)find_node(thread_table, udp_sockfd)->container ;
    while(1){
        pthread_mutex_lock(&thread_table_lock);
        if(thread_ptr->is_close_requested  == 1){
            debug_log(udp_sockfd,"\nThread X status set for termination ");
            exit_status = 1 ;
            socket_container->receive_buffer = delete_tree(socket_container->receive_buffer);
            socket_container->receive_message_id = delete_tree(socket_container->receive_message_id);
            socket_container->unacknowledged_message = delete_tree(socket_container->unacknowledged_message);
            debug_log(udp_sockfd,"\nThread resource deleted ")
            pthread_cond_signal(&thread_table_cond);
        }
        pthread_mutex_unlock(&thread_table_lock);
        if(exit_status == 1){
            break;
        }

        // updating the recieved message list and send message list

        // here is the part where all the delete and create stuff should happen
    }

    debug_log(udp_sockfd,"\nReturning the thread X");
    FLUSH();

    return NULL;
    
}

// TODO : Rethink if this is the perfect thing for you to do
// r_socket is a wrapper function for socket (sys/socket) for r_socket library
int r_socket(int domain, int type, int protocol){
    debug_log(0,"\nCreating a r_socket ");
    // creation of the udp port 
    int udp_socket = 0;
    udp_socket = socket(domain, type, protocol);
    if(udp_socket == -1){
        return -1;
    }
    debug_log(udp_socket,"\nFile Descriptor created ");;

    // creating a socket container for this sockets data  
    r_socket_container * socket_container =  (r_socket_container *)malloc(sizeof(r_socket_container));
    socket_container->udp_sockfd = udp_socket;
    socket_container->receive_buffer = NULL;
    socket_container->unacknowledged_message = NULL;
    socket_container->receive_message_id = NULL;
    debug_log(udp_socket,"\nSocket Container created ");
    
    // creating a thread container for sockets thread x
    thread_container * thread_ptr = (thread_container *)malloc(sizeof(thread_container));
    thread_ptr->is_close_requested = 0;
    pthread_attr_init(&thread_ptr->attr);
    pthread_attr_setdetachstate(&thread_ptr->attr, PTHREAD_CREATE_DETACHED);
    debug_log(udp_socket,"\nThread container created ");

    int ret = pthread_create(&thread_ptr->id, &thread_ptr->attr, X_routine,(void *)socket_container);
    
    // check for error while creating a thread 
    if(ret == -1){

        // printing error and removing all the data allocated for that thread
        perror("Unable to create Thread X ");
        close(udp_socket);

        // destroying created locks and attribtues (local)
        pthread_attr_destroy(&thread_ptr->attr);

        // freeing the allocated space (local)
        free(thread_ptr);
        free(socket_container);

        return -1;
    }

    // adding the socket to the socket_table (global)
    pthread_mutex_lock(&socket_table_lock);
    socket_table = insert_node(socket_table, udp_socket, (void *)socket_container);
    pthread_mutex_unlock(&socket_table_lock);
    debug_log(udp_socket,"\nEntry added to socket_table ");

    // adding thread to the thread_table (global)
    pthread_mutex_lock(&thread_table_lock);
    thread_table = insert_node(thread_table, udp_socket, (void *)thread_ptr);
    pthread_mutex_unlock(&thread_table_lock);
    debug_log(udp_socket,"\nEntry added to thread_table ");
    debug_log(udp_socket,"\nr_socket created ");
    return udp_socket;
}


// TODO : figure out if I am missing something or this function is this simple in existance 

// r_bind is a wrapper function for bind (sys/socket) for r_socket library
int r_bind(int sockfd, const struct sockaddr * addr, socklen_t len){
    debug_log(sockfd,"\nr_socket bound ");
    return bind(sockfd, addr, len);
}

// r_close if the wrapper function for close (unistd) for r_socket library
// NOTE : At any given point of time only one of the close, socket, bind, recv, send (or other API) function can be called thus no problem of locking the resource shared between the API calls only
// NOTE : API calls are the only thread that can change the state of the socket_table and thread_table , thread_x can only read from these tables

int r_close(int sockfd){
    // closing the socket 
    close(sockfd);
    debug_log(sockfd,"\nFile descriptor closed ");
    // getting the mutex lock for thread table and setting the thread exis status to exit now
    int thread_id =  0;

    pthread_mutex_lock(&thread_table_lock);

    debug_log(sockfd,"\nAcquired Mutation lock");
    fflush(stdin);fflush(stdout);fflush(stderr);
    Node * node = find_node(thread_table, sockfd);
    thread_container * thread_ptr = node->container;
    thread_id = thread_ptr->id;
    thread_ptr->is_close_requested = 1 ;
    debug_log(sockfd,"\nis_close_requested set to true ");
    fflush(stdin);fflush(stdout);fflush(stderr);

    pthread_cond_wait(&thread_table_cond, &thread_table_lock);
    pthread_attr_destroy(&thread_ptr->attr);
    debug_log(sockfd,"\ndestroying attr for thread ");

    thread_table = delete_node(thread_table, sockfd);
    debug_log(sockfd, "\nDeleted corresponding record from the table");
    FLUSH();
    pthread_mutex_unlock(&thread_table_lock);
    debug_log(sockfd, "\nWaiting for thread to get over ");
    FLUSH();
    // pthread_join(thread_id, NULL);
    debug_log(sockfd,"\nJoined the exiting thread ");
    FLUSH();
    pthread_mutex_lock(&socket_table_lock);
    socket_table = delete_node(socket_table, sockfd);
    pthread_mutex_unlock(&socket_table_lock);
    debug_log(sockfd,"\nRemoved the socket entry from the table ");
    return 0;
}


// TODO : create destructor for the containers beacause they have global data like mutex and attribs (and assign the container value to NULL for table deletion)
// TODO : implement the close functionality completely (with removal of socket_container and thread_ptr from the global tables)
// TODO : create a test environment (with a simple client (r_socket should be used on the server side))
// TODO : test this with simple socket and close (if they are not working nothing else will)