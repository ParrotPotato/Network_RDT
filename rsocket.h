#ifndef R_SOCKET_H
#define R_SOCKET_H

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

#define SOCK_MRP 10101

int r_socket(int domain, int type, int protocol);
int r_bind(int sockfd, const struct sockaddr * addr, socklen_t len);
int r_connect(int sockfd, const struct sockaddr * addr, socklen_t len);
int r_close(int sockfd);
int r_sendto(int sockfd, const void * message, size_t len, int flags, const struct sockaddr * dest_addr, socklen_t addrlen);
int r_recvfrom(int sockfd, void * message, size_t len, int flags, struct sockaddr * dest_addr, socklen_t * addrlen);
float getRatioForTransmission(int length);

#endif
