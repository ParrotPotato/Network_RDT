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
#include "rcontainer.h"

#define TESTING

#ifdef TESTING 

pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;

#define x_debug_log(...)  pthread_mutex_lock(&printLock);printf("\033[33m");printf(__VA_ARGS__);printf("\033[0m");pthread_mutex_unlock(&printLock)
#define debug_log(...)  pthread_mutex_lock(&printLock);printf("\033[34zzm");printf(__VA_ARGS__);printf("\033[0m");pthread_mutex_unlock(&printLock)
#define FLUSH()  fflush(stdin);fflush(stdout);fflush(stderr)

#else

#define x_debug_log(...) 
#define debug_log(...) 
#define FLUSH() 

#endif

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

    pthread_mutex_lock(&socket_container->lock);
    if(socket_container->is_usable == 0){
        x_debug_log("\n(socket : %d) Thread waiting for socket binding", udp_sockfd);
        pthread_cond_wait(&socket_container->cond, &socket_container->lock);
    }
    pthread_mutex_unlock(&socket_container->lock);

    int exit_status = 0 ;
    x_debug_log("\n(socket : %d) Thread X running ",udp_sockfd);
    thread_container * thread_ptr = (thread_container *)find_node(thread_table, udp_sockfd)->container ;

    int recv_n = 0 ;
    char buffer[101];

    
    while(1){
        pthread_mutex_lock(&thread_table_lock);
        if(thread_ptr->is_close_requested  == 1){
            x_debug_log("\n(socket : %d) Thread X status set for termination ", udp_sockfd);
            exit_status = 1 ;
            socket_container->receive_buffer = delete_tree(socket_container->receive_buffer);
            socket_container->receive_message_id = delete_tree(socket_container->receive_message_id);
            socket_container->unacknowledged_message = delete_tree(socket_container->unacknowledged_message);
            pthread_mutex_destroy(&socket_container->lock);
            pthread_cond_destroy(&socket_container->cond);
            x_debug_log("\n(socket : %d) Thread resource deleted",udp_sockfd);
            pthread_cond_signal(&thread_table_cond);
        }
        pthread_mutex_unlock(&thread_table_lock);
        if(exit_status == 1){
            break;
        }
        
        pthread_mutex_lock(&socket_container->lock);
        
        while(1){
            break;
        }
        pthread_mutex_unlock(&socket_container->lock);

    }

    x_debug_log("\n(socket : %d) Returning the thread X",udp_sockfd);
    FLUSH();

    return NULL;
    
}


// r_socket is a wrapper function for socket (sys/socket) for r_socket library
int r_socket(int domain, int type, int protocol){
    debug_log("\n(socket : %d) Creating a r_socket ", 0);
    // creation of the udp port 
    int udp_socket = 0;
    udp_socket = socket(domain, type, protocol);
    if(udp_socket == -1){
        return -1;
    }
    debug_log("\n(socket : %d) File Descriptor created ",udp_socket);;

    // creating a socket container for this sockets data  
    r_socket_container * socket_container =  (r_socket_container *)malloc(sizeof(r_socket_container));
    socket_container->udp_sockfd = udp_socket;
    socket_container->receive_buffer = NULL;
    socket_container->unacknowledged_message = NULL;
    socket_container->receive_message_id = NULL;
    pthread_mutex_init(&socket_container->lock, NULL);
    pthread_cond_init(&socket_container->cond, NULL);
    socket_container->counter = 0;
    socket_container->is_usable = 0;
    debug_log("\n(socket : %d) Socket Container created ", udp_socket);
    
    // creating a thread container for sockets thread x
    thread_container * thread_ptr = (thread_container *)malloc(sizeof(thread_container));
    thread_ptr->is_close_requested = 0;
    pthread_attr_init(&thread_ptr->attr);
    pthread_attr_setdetachstate(&thread_ptr->attr, PTHREAD_CREATE_DETACHED);
    debug_log("\n(socket : %d) Thread container created ", udp_socket);

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
    debug_log("\n(socket : %d) Entry added to socket_table ", udp_socket);

    // adding thread to the thread_table (global)
    pthread_mutex_lock(&thread_table_lock);
    thread_table = insert_node(thread_table, udp_socket, (void *)thread_ptr);
    pthread_mutex_unlock(&thread_table_lock);
    debug_log("\n(socket : %d) Entry added to thread_table ", udp_socket);
    debug_log("\n(socket : %d) r_socket created ", udp_socket);
    return udp_socket;
}


// TODO : figure out if I am missing something or this function is this simple in existance 

// r_bind is a wrapper function for bind (sys/socket) for r_socket library
int r_bind(int sockfd, const struct sockaddr * addr, socklen_t len){
    r_socket_container * socket_container;
    
    pthread_mutex_lock(&socket_table_lock);
    socket_container = (r_socket_container *)find_node(socket_table, sockfd)->container; 
    pthread_mutex_unlock(&socket_table_lock);

    pthread_mutex_lock(&socket_container->lock);

    int ret_val = bind(sockfd, addr, len);
    socket_container->is_usable = 1 ;
    pthread_cond_signal(&socket_container->cond);

    pthread_mutex_unlock(&socket_container->lock);

    debug_log("\n(socket : %d) r_socket bound ", sockfd);
    return ret_val;
}

int r_connect(int sockfd, const struct sockaddr * addr, socklen_t len){
    r_socket_container * socket_container;

    pthread_mutex_lock(&socket_table_lock);
    socket_container = (r_socket_container *)find_node(socket_table, sockfd)->container; 
    pthread_mutex_unlock(&socket_table_lock);

    pthread_mutex_lock(&socket_container->lock);

    int ret_val = connect(sockfd, addr, len);
    socket_container->is_usable = 1 ;
    pthread_cond_signal(&socket_container->cond);

    pthread_mutex_unlock(&socket_container->lock);

    debug_log("\n(socket : %d) r_socket connected ", sockfd);
    return ret_val;
}


// r_close if the wrapper function for close (unistd) for r_socket library
// NOTE : At any given point of time only one of the close, socket, bind, recv, send (or other API) function can be called thus no problem of 
        //locking the resource shared between the API calls only
// NOTE : API calls are the only thread that can change the state of the socket_table and thread_table , thread_x can only read from these tables



int r_close(int sockfd){
    // closing the socket 
    close(sockfd);
    debug_log("\n(socket : %d) File descriptor closed ", sockfd);
    // getting the mutex lock for thread table and setting the thread exis status to exit now
    int thread_id =  0;

    pthread_mutex_lock(&thread_table_lock);

    debug_log("\n(socket : %d) Acquired Mutation lock", sockfd);
    fflush(stdin);fflush(stdout);fflush(stderr);
    
    thread_container * thread_ptr = (thread_container *)find_node(thread_table, sockfd)->container;
    thread_id = thread_ptr->id;
    thread_ptr->is_close_requested = 1 ;
    debug_log("\n(socket : %d) is_close_requested set to true ", sockfd);
    fflush(stdin);fflush(stdout);fflush(stderr);

    pthread_cond_wait(&thread_table_cond, &thread_table_lock);

    pthread_attr_destroy(&thread_ptr->attr);
    debug_log("\n(socket : %d) destroying attr for thread ", sockfd);

    thread_table = delete_node(thread_table, sockfd);
    debug_log("\n(socket : %d) Deleted corresponding record from the table", sockfd);
    FLUSH();
    pthread_mutex_unlock(&thread_table_lock);
    //debug_log(sockfd, "\nWaiting for thread to get over ");
    FLUSH();
    // pthread_join(thread_id, NULL);
    //debug_log(sockfd,"\nJoined the exiting thread ");
    FLUSH();
    pthread_mutex_lock(&socket_table_lock);
    socket_table = delete_node(socket_table, sockfd);
    pthread_mutex_unlock(&socket_table_lock);

    debug_log("\n(socket : %d) Removed the socket entry from the table ", sockfd);
    debug_log("\n(socket : %d) r_socket closed", sockfd);
    return 0;
}

// TODO : with an open mind check the functionality of sendto I think the basics are met here and are done and dusted
// TODO : replace all the buffer declaration with dynamic calls
int r_sendto(int sockfd, const void * mess, size_t len, int flags, const struct sockaddr * dest_addr, socklen_t addrlen){
    char * message = (char *)mess;
    struct sockaddr_in * server = (struct sockaddr_in *) dest_addr; 
    // getting the socket_container for this sockfd 
    r_socket_container * socket_ptr = (r_socket_container *)find_node(socket_table, sockfd)->container;
    if(socket_ptr == NULL){
        return -1;
    }

    // creating buffer 
    char * buffer = (char *)malloc(sizeof(char) * len + 1);
    buffer[0] = (char)socket_ptr->counter;
    
    strcpy(buffer + 1, message);

    // container_ptr for a new entry in unacknowledged_message
    char * ip_buffer = (char *)malloc(sizeof(char) * 16);
    strcpy(ip_buffer, inet_ntoa(server->sin_addr));
    unsigned short port = (server->sin_port) ;
    port = ntohs(port);
    debug_log("\nIP : %s\nPort : %d\n", ip_buffer, (int)port);
    unacknowledged_message_container * container_ptr = (unacknowledged_message_container *)malloc(sizeof(unacknowledged_message_container)) ;
    container_ptr->buffer = buffer ;
    container_ptr->message_id = socket_ptr->counter ;
    container_ptr->len = len + 1 ;
    container_ptr->ip = ip_buffer;
    container_ptr->port = port;
    time(&container_ptr->sending_time);

    printf("\n\nTrying to send : %s\n\n", message);
    printf("\n\nGenerated String : %s\n\n", buffer + 1 );

    debug_log("\n(socket : %d)Adding Unacknowledged Message", sockfd);

    print_unacknowledged_message(container_ptr);
    
    // getting current time (in seconds)

    // sending the message using UDP port 
    int send_n = sendto(sockfd, buffer, len + 1, flags, dest_addr, addrlen) ;
    if(send_n == -1){
        free(container_ptr) ;
        free(buffer) ;
        return -1;
    }

    // setting mutation lock on socket_ptr (socket container corresponding to that socket)
    pthread_mutex_lock(&socket_ptr->lock);
    socket_ptr->unacknowledged_message = insert_node(socket_ptr->unacknowledged_message , container_ptr->sending_time, (void *)container_ptr);
    socket_ptr->counter = socket_ptr->counter+1 ;
    pthread_mutex_unlock(&socket_ptr->lock);

    return send_n;
}

int r_recvfrom(int sockfd, void * message, size_t len, int flags, const struct sockaddr * dest_addr, socklen_t * addrlen){
    
}



// NOTE : inet_ntoa() returns a char * in its own function stack (DO NOT DELETE)
// TODO : implement the IP and PORT container for unacknowledge messages 