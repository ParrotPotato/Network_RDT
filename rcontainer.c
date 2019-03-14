#include "rcontainer.h"

void print_receive_buffer(receive_buffer_container * ptr){
    if(ptr == NULL)
        return;
    printf("\nMessage ID : %d\nBuffer : %s", ptr->message_id, ptr->buffer);
}


void print_unacknowledged_message(unacknowledged_message_container * ptr){
    if(ptr == NULL)
        return;
    printf("\nSending Time : %ld\nMessage ID : %d\nMessage : %s\nLen : %d\nIP : %s\nPort : %d", 
                                                                            ptr->sending_time, 
                                                                            ptr->message_id, 
                                                                            ptr->buffer, 
                                                                            ptr->len,
                                                                            ptr->ip, 
                                                                            (int)ptr->port);
}

void print_receive_message_id(receive_message_id_container * ptr){
    if(ptr == NULL)
        return;
    printf("\nMessage ID : %d", ptr->message_id);
}