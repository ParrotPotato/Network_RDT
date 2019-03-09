#ifndef HEADER_RSOCKETCONTAINER
#define HEADER_RSOCKETCONTAINER

// NOTE : for all the function calls assign the value to itself
// NOTE : think of adding #dif for insert and delete functionality to avoid the self assignment 

struct def_Node {
    int key;
    void * container;

    struct def_Node * left;
    struct def_Node * right;
    int node_height;
};

typedef struct def_Node Node;
typedef Node * Table;

extern int node_height(Node * root);
extern int max(int x,int y);
extern int get_node_balance(Node * root);
extern Node * create_Node_empty();
extern Node * create_Node(int key, void *container);
extern Node * rotate_node_right(Node * y);
extern Node * rotate_node_left(Node * x);
extern Node * insert_node(Node * root, int key, void * container);
extern Node * delete_node(Node * root, int key);
extern Node * delete_tree(Node * root);
extern Node * find_node(Node * root, int key);

extern void print_key_sequence(Node * root);
extern void print_all_leaf(Node * root);

#endif