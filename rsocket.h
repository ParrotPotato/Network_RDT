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

extern int r_socket(int domain, int type, int protocol);
extern int r_bind(int sockfd, const struct sockaddr * addr, socklen_t len);
extern int r_connect(int sockfd, const struct sockaddr * addr, socklen_t len);
extern int r_close(int sockfd);
extern int r_sendto(int sockfd, const void * message, size_t len, int flags, const struct sockaddr * dest_addr, socklen_t addrlen);
extern int r_recvfrom(int sockfd, const void * message, size_t len, int flags, struct sockaddr * dest_addr, socklen_t * addrlen);

#endif