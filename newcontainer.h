#ifndef HEADER_CONTAINER
#define HEADER_CONTAINER

struct def_Node{
    long int key;
    void * container;
    struct def_Node *  next;
};
typedef struct def_Node Node;


#endif