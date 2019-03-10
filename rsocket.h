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
extern int r_close(int sockfd);

#endif