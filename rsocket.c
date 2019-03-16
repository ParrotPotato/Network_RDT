// TODO : check for strcpy conflicts with other message formats (suppose someone is sending a 0 or the id is 0)
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
#include <sys/select.h>

#include "rlistcontainer.h"


#define TESTING

#ifdef TESTING 

pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;

#define x_debug_log(...)  pthread_mutex_lock(&printLock);printf("\033[33m");printf(__VA_ARGS__);printf("\033[0m");pthread_mutex_unlock(&printLock);fflush(stdout);fflush(stdin)
#define debug_log(...)  pthread_mutex_lock(&printLock);printf("\033[36m");printf(__VA_ARGS__);printf("\033[0m");pthread_mutex_unlock(&printLock);fflush(stdout);fflush(stdin)
#define FLUSH()  fflush(stdin);fflush(stdout);fflush(stderr)

#else

#define x_debug_log(...) 
#define debug_log(...) 
#define FLUSH() 

#endif


struct def_receive_buffer_container{
    int message_id;
    char buffer[100];
    struct sockaddr addr;
    int len;
    int addrlen;
};

struct def_unacknowledge_message_container{
    int message_id;
    char buffer[100];
    struct sockaddr addr;
    time_t sending_time;
};

struct def_recieved_message_container{
    int message_id;
};

struct def_socket_container{
    Table receive_buffer;
    Table unacknowledged_message;
    Table receive_message;

    int sockfd;
    int message_counter;
    int is_usable;
    int is_close_requested;

    pthread_mutex_t lock;
    pthread_cond_t cond;
};

struct def_thread_container{
    pthread_t thread_id;
    pthread_mutex_t lock;
    pthread_attr_t attr;
};

typedef struct def_receive_buffer_container receive_buffer_container ;
typedef struct def_unacknowledge_message_container unacknowledge_message_container ;
typedef struct def_recieved_message_container recieved_message_container ;
typedef struct def_socket_container socket_container ;
typedef struct def_thread_container thread_container ;

receive_buffer_container * new_receive_buffer_container(int message_id, char buffer[100]){
    receive_buffer_container * buffer_ptr = (receive_buffer_container *)malloc(sizeof(receive_buffer_container));
    buffer_ptr->message_id = message_id;
    strcpy(buffer_ptr->buffer, buffer);

    debug_log("receive buffer container constructed \n");
    return buffer_ptr;
}

unacknowledge_message_container * new_unacknowledge_message_container(){
    unacknowledge_message_container * ptr = (unacknowledge_message_container *)malloc(sizeof(unacknowledge_message_container));
    memset(ptr, 0, sizeof(unacknowledge_message_container));

    debug_log("unacknowledge message container constructed \n");
    return ptr;
}

int send_unacknowledge_message(int sockfd, unacknowledge_message_container * ptr){
    char * buffer = (char *)malloc(sizeof(char) * (strlen(ptr->buffer) + 2 ));
    int i=0 ;
    for(i =0 ; i < strlen(ptr->buffer) ; i++){
        buffer[i] = ptr->buffer[i];
    }
    buffer[i] = '\0';

    buffer[0] = (char)ptr->message_id;
    int send_n = sendto(sockfd, buffer, strlen(ptr->buffer) + 2, 0, &ptr->addr, sizeof(ptr->addr));
    return send_n;
}

int x_send_unacknowledge_message(int sockfd, unacknowledge_message_container * ptr){
    x_debug_log("sending unacknowledge message using the send_function()\n");
    char * buffer = (char *)malloc(sizeof(char) * (strlen(ptr->buffer) + 2));
    int i=1 ;
    for(i =1 ; i < strlen(ptr->buffer) ; i++){
        buffer[i] = ptr->buffer[i];
    }
    buffer[i] = '\0';
    x_debug_log("retransmission message generated \n");
    buffer[0] = (char)ptr->message_id;
    int send_n = sendto(sockfd, buffer, strlen(ptr->buffer) + 2, 0, &ptr->addr, sizeof(ptr->addr));
    free(buffer);
    return send_n;
}

void sockaddr_copy(struct sockaddr * base, struct sockaddr * from){
    base->sa_family = from->sa_family;
    int i=0 ;
    for(i =0 ; i < 14 ; i++){
        base->sa_data[i] = from->sa_data[i];
    }
}

socket_container * new_socket_container(int sockfd){
    socket_container * socket_ptr = (socket_container * )malloc(sizeof(socket_container));

    socket_ptr->message_counter = 0;
    socket_ptr->is_usable = 0;
    socket_ptr->is_close_requested =  0;

    socket_ptr->receive_buffer = NULL;
    socket_ptr->receive_message = NULL;
    socket_ptr->unacknowledged_message = NULL;
    
    socket_ptr->sockfd = sockfd;

    pthread_mutex_init(&socket_ptr->lock, NULL);
    pthread_cond_init(&socket_ptr->cond, NULL);

    debug_log("new socket container constructed \n");
    return socket_ptr;
}

thread_container * new_thread_container(){

    thread_container * thread_ptr = (thread_container *)malloc(sizeof(thread_container));
    pthread_mutex_init(&thread_ptr->lock, NULL);
    pthread_attr_init(&thread_ptr->attr);
    pthread_attr_setdetachstate(&thread_ptr->attr, PTHREAD_CREATE_DETACHED);

    debug_log("new thread container constructed \n");
    return thread_ptr;
}


Table socket_table = NULL;
Table thread_table = NULL;


pthread_mutex_t socket_table_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_table_lock = PTHREAD_MUTEX_INITIALIZER;


pthread_cond_t socket_table_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t thread_table_cond = PTHREAD_COND_INITIALIZER;

void * thread_x_routine(void * args);


int r_socket(int domain, int type, int protocol){
    int sockfd = socket(domain, type, protocol);
    if(sockfd == -1)
        return -1;

    socket_container * socket_ptr = new_socket_container(sockfd);
    thread_container * thread_ptr = new_thread_container();

    int ret = pthread_create(&thread_ptr->thread_id, &thread_ptr->attr, thread_x_routine, (void *)socket_ptr);
    if(ret == -1){
        pthread_mutex_destroy(&socket_ptr->lock);

        pthread_mutex_destroy(&thread_ptr->lock);
        pthread_attr_destroy(&thread_ptr->attr);
        
        free(socket_ptr);
        free(thread_ptr);

        return -1;
    }
    
    pthread_mutex_lock(&socket_table_lock);
    socket_table = insert_node(socket_table, sockfd, (void *)socket_ptr);
    pthread_mutex_unlock(&socket_table_lock);

    pthread_mutex_lock(&thread_table_lock);
    thread_table = insert_node(thread_table, sockfd, (void *)thread_ptr);
    pthread_mutex_unlock(&thread_table_lock);

    return sockfd;
}

int r_bind(int fd, const struct sockaddr * addr, socklen_t len){
    pthread_mutex_lock(&socket_table_lock);
    socket_container * socket_ptr = (socket_container *)find_node(socket_table, fd)->container;
    pthread_mutex_unlock(&socket_table_lock);

    if(bind(fd, addr, len)  < 0){
        return -1 ;
    }
    // debug_log("\nfetching lock for socket_ptr");
    pthread_mutex_lock(&socket_ptr->lock);
    socket_ptr->is_usable = 1 ;
    debug_log("setting thread free\n");
    pthread_cond_signal(&socket_ptr->cond);
    pthread_mutex_unlock(&socket_ptr->lock);

    return 1;
}

int r_connect(int fd, const struct sockaddr * addr, socklen_t len){
    pthread_mutex_lock(&socket_table_lock);
    socket_container * socket_ptr = (socket_container *)find_node(socket_table, fd)->container;
    pthread_mutex_unlock(&socket_table_lock);

    if(connect(fd, addr, len)  < 0){
        return -1 ;
    }

    pthread_mutex_lock(&socket_ptr->lock);
    socket_ptr->is_usable = 1 ;
    pthread_cond_signal(&socket_ptr->cond);
    pthread_mutex_unlock(&socket_ptr->lock);

    return 1;
}

int r_sendto(int fd, const void * buf, size_t len, int flag, const struct sockaddr * addr, socklen_t addrlen){
    pthread_mutex_lock(&socket_table_lock);
    socket_container * socket_ptr = (socket_container *)find_node(socket_table, fd)->container;
    pthread_mutex_unlock(&socket_table_lock);
    if(socket_ptr == NULL){
        return -1;
    }
    
    unacknowledge_message_container * unack_message_ptr = new_unacknowledge_message_container();
    strcpy(unack_message_ptr->buffer, (char *)buf);
    unack_message_ptr->message_id = socket_ptr->message_counter;
    sockaddr_copy(&unack_message_ptr->addr, addr);
    time(&unack_message_ptr->sending_time);

    int ret = send_unacknowledge_message(fd, unack_message_ptr);
    if(ret == -1){
        free(unack_message_ptr);
        return -1;
    }
    
    socket_ptr->message_counter++;

    // debug_log("\nfetching lock for socket_ptr");
    pthread_mutex_lock(&socket_ptr->lock);
    socket_ptr->unacknowledged_message = insert_node(socket_ptr->unacknowledged_message, unack_message_ptr->message_id, unack_message_ptr);
    pthread_mutex_unlock(&socket_ptr->lock);

    return ret;
}

int r_close(int fd){
    pthread_mutex_lock(&socket_table_lock);
    socket_container * socket_ptr = (socket_container *)find_node(socket_table, fd)->container;
    pthread_mutex_unlock(&socket_table_lock);
    if(socket_ptr == NULL){
        return -1;
    }
    // debug_log("\nfetching lock for socket_ptr");

    pthread_mutex_lock(&socket_ptr->lock);
    socket_ptr->is_close_requested = 1 ;
    debug_log("requesting thread to close\n");
    pthread_cond_wait(&socket_ptr->cond, &socket_ptr->lock);
    pthread_mutex_unlock(&socket_ptr->lock);

    pthread_mutex_destroy(&socket_ptr->lock);
    pthread_cond_destroy(&socket_ptr->cond);

    pthread_mutex_lock(&socket_table_lock);
    socket_table = delete_node(socket_table, fd);
    pthread_mutex_unlock(&socket_table_lock);

    pthread_mutex_lock(&thread_table_lock);
    thread_table = delete_node(thread_table, fd);
    pthread_mutex_unlock(&thread_table_lock);

    return close(fd);
}

void retransmit(Node * ptr, void * args){
    unacknowledge_message_container * unack_message = (unacknowledge_message_container *)ptr->container;
    if(time(NULL) - unack_message->sending_time > 2){
        x_debug_log("Retransmitting    : %s\n", unack_message->buffer);
        x_debug_log("Retransmitting to : %d\n", *((int *)args));
        x_send_unacknowledge_message(*((int *)args), unack_message);
        unack_message->sending_time = time(NULL);
        x_debug_log("retransmitting message\n");
    }
    return;
}

void HandleRetansmission(socket_container * ptr){
    pthread_mutex_lock(&ptr->lock);
    x_debug_log("Handling retransmission\n");
    apply_to_all_nodes(ptr->unacknowledged_message, retransmit, (void *)&ptr->sockfd);
    pthread_mutex_unlock(&ptr->lock);

}

// TODO : check for acknowledgment and user message 
// TODO : Implement the message_id buffer 
// TODO : Implement the checking for duplicate messages
// TODO : HandleAppMsgRecv for handling the application messages and 
// TODO : HandleACKMsgRecv for handling the ACK messages


void HandleReceive(socket_container * ptr){
    
    x_debug_log("handling receive \n");
    char tempbuffer[101];
    int len = sizeof(struct sockaddr );
    struct sockaddr temp ;
    
    int recv_n = recvfrom(ptr->sockfd, tempbuffer, 101, 0, &temp, &len);
    receive_buffer_container * recv_buffer = new_receive_buffer_container((int)tempbuffer[0], tempbuffer + 1 );
    recv_buffer->addrlen = len;
    recv_buffer->len = recv_n;
    recv_buffer->addr = temp;
    
    pthread_mutex_lock(&ptr->lock);
    ptr->receive_buffer = insert_node(ptr->receive_buffer, recv_buffer->message_id, (void *)recv_buffer);

    x_debug_log("Inserted a received message in the buffer \n");
    x_debug_log("Buffer   : %s \n", recv_buffer->buffer);
    x_debug_log("Buffer-id: %d \n", recv_buffer->message_id);

    pthread_cond_signal(&ptr->cond);
    pthread_mutex_unlock(&ptr->lock);

    // NOTE : insert the message id into the recieve message table (which has all the recieved ids of the messages)

    
    return;
}

void * thread_x_routine(void * args){
    x_debug_log("thread created and running\n");
    socket_container * socket_ptr = (socket_container * )args;
    
    // x_debug_log("\nfetching lock for socket_ptr");
    pthread_mutex_lock(&socket_ptr->lock);
    if(socket_ptr->is_usable == 0){
        x_debug_log("thread waiting for socket to become usable\n");
        pthread_cond_wait(&socket_ptr->cond, &socket_ptr->lock);
        x_debug_log("thread socket became usable\n");
    }
    pthread_mutex_unlock(&socket_ptr->lock);

    fd_set rset;
    FD_ZERO(&rset);

    int maxfd = socket_ptr->sockfd + 1 , select_ret;
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int quit_condition = 0 ;
    while(quit_condition == 0){
        // x_debug_log("\nlocking socket_container\n");
        pthread_mutex_lock(&socket_ptr->lock);
        // x_debug_log("\nsocket container locked\n");
        if(socket_ptr->is_close_requested == 1){
            quit_condition = 1;
            
            // TODO : waiting till all the messages have ben recieved

            socket_ptr->receive_buffer = delete_list(socket_ptr->receive_buffer);
            socket_ptr->receive_message = delete_list(socket_ptr->receive_message);
            socket_ptr->unacknowledged_message = delete_list(socket_ptr->unacknowledged_message);

            x_debug_log("close request handled by thread\n");
            pthread_cond_signal(&socket_ptr->cond);
        }
        pthread_mutex_unlock(&socket_ptr->lock);
        if(quit_condition == 1 )
        {
            break;
        }

        // select statements 

        FD_SET(socket_ptr->sockfd, &rset);
        select_ret = select(maxfd, &rset, NULL, NULL, &timeout);
        if(select_ret == -1){
            x_debug_log("Something went wrong somewhere \n");
        }
        else if(select_ret == 0){
            x_debug_log("Time out occured\n");

            HandleRetansmission(socket_ptr);
            
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
        }
        else if(FD_ISSET(socket_ptr->sockfd, &rset)){
            x_debug_log("something received \n");
            HandleReceive(socket_ptr);
        }
    }
    return NULL;
}


int r_recvfrom(int fd, void * buffer, size_t len, int flag, struct sockaddr * addr , socklen_t * addrlen){
    pthread_mutex_lock(&socket_table_lock);
    socket_container * socket_ptr = (socket_container *)find_node(socket_table, fd)->container;
    pthread_mutex_unlock(&socket_table_lock);

    if(socket_ptr == NULL)
        return -1;

    char * buff = (char *)buffer;

    pthread_mutex_lock(&socket_ptr->lock);
    if(socket_ptr->receive_buffer == NULL){
        debug_log("waiting for receving a message\n");
        pthread_cond_wait(&socket_ptr->cond, &socket_ptr->lock);
        debug_log("Message recieved\n");
    }
    pthread_mutex_unlock(&socket_ptr->lock);

    int i =0 ; 
    for(i=0 ;i < len ; i++){
        buff[i] = ((receive_buffer_container *)socket_ptr->receive_buffer->container)->buffer[i];
    }
    
    for(i=0 ; i < 14 ; i++){
        addr->sa_data[i] = ((receive_buffer_container *)socket_ptr->receive_buffer->container)->addr.sa_data[i];
    }
    addr->sa_family  = ((receive_buffer_container *)socket_ptr->receive_buffer->container)->addr.sa_family;

    *addrlen = ((receive_buffer_container *)socket_ptr->receive_buffer->container)->addrlen;
    int retvalue = ((receive_buffer_container *)socket_ptr->receive_buffer->container)->len;

    pthread_mutex_lock(&socket_ptr->lock);
    socket_ptr->receive_buffer = delete_node(socket_ptr->receive_buffer, socket_ptr->receive_buffer->key);
    pthread_mutex_unlock(&socket_ptr->lock);

    return retvalue;
}