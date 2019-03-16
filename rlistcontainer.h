#ifndef HEADER_CONTAINER
#define HEADER_CONTAINER


struct def_Node {
    int key;
    void * container;
    struct def_Node * next;
};
typedef struct def_Node Node;
typedef struct def_Node * Table;


extern Node * create_Node();
extern Node * insert_node(Node * root, int key, void * container);
extern Node * delete_node(Node * root, int key);
extern Node * find_node(Node * root, int key);
extern Node * delete_list(Node * root);
extern void * apply_to_all_nodes(Node * root, void function_ptr(Node * ptr, void * args), void * args);
extern void print_list(Node * root);


#endif