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
#include <termios.h>

#define PORT 10000

void run(){
    printf("YO \n");
}

void runagain(){
    printf("YOLO \n");
}

void callback(void ptr()){
    ptr();
}

int main(int argc, char **argv)
{
    callback(run);
    callback(runagain);
    return 0;
}