#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 


void * thread_function(void * attribute){
    int * ptr ;
    ptr = malloc(sizeof(int));
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    printf("Thread running \n");
    sleep(1);
    *ptr = 1 ;
    return (void *)ptr;
}

pthread_t create_thread(){
    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&thread_id, &attr, thread_function, NULL);

    printf("create_thread_running \n");
    sleep(1);
    printf("create_thread_exiting\n");
    return thread_id;
}

int main(){
    printf("Running main \n");

    pthread_t thread_id = create_thread();
    
    int * ptr = 0;
    pthread_join(thread_id, (void **)&ptr);

    printf("Return value of thread : %d", *ptr);
    
    return 0;
}