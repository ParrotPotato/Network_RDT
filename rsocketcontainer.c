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

#include "rsocketcontainer.h"

#define debug_flag 0
#define debug_log(...) if(debug_flag == 1 ) printf(__VA_ARGS__)
#define debug_comment //
#define FLUSH() debug_comment fflush(stdin);fflush(stdout);fflush(stderr)

int node_height(Node * root){
    if(root == NULL){
        return 0;
    }
    return root->node_height;
}

int max_val(int x,int y){
    if(x > y )
        return x;
    return y;
}

Node * minimum_node(Node * root){
    if(root == NULL){
        return NULL;
    }

    while(root->left != NULL){
        root = root->left;
    }

    return root;
}

int get_node_balance(Node * root){
    if(root == NULL)
        return 0;
    return node_height(root->left) - node_height(root->right);
}

Node * create_Node_empty(){
    Node * ptr = (Node *)malloc(sizeof(Node));
    ptr->key = -1;
    ptr->container = NULL;
    ptr->node_height = 1;

    ptr->left = NULL;
    ptr->right = NULL;

    return ptr;
}

Node * create_Node(long int key, void *container){
    Node * ptr = (Node *)malloc(sizeof(Node));
    ptr->key = key;
    ptr->container = container;
    ptr->node_height = 1;

    ptr->left = NULL;
    ptr->right = NULL;

    return ptr;
}


Node * rotate_node_right(Node * y){
    //printf("\nBefore right rotation height : %d", node_height(y));

    Node * x = y->left;
    Node * T2 = x->right;

    x->right = y;
    y->left = T2;

    y->node_height = 1 + max_val(node_height(y->left), node_height(y->right));
    x->node_height = 1 + max_val(node_height(x->left), node_height(x->right));

    //printf("\nAfter right rotation height : %d",node_height(x));
    return x;
}

Node * rotate_node_left(Node * x){
    //printf("\nBefore left rotation height : %d", node_height(x));

    Node * y = x->right;
    Node * T2 = y->left;


    y->left = x;
    x->right = T2;

    //printf("\nBefore Updation Right of y : %d",node_height( y->right));
    //printf("\nBefore Updation Left of y : %d",node_height (y->left));

    // printf("\nBefore Updation Right of x : %d", node_height(x->right));
    // printf("\nBefore Updation Left of x : %d", node_height(x->left));


    x->node_height = 1 + max_val(node_height(x->left), node_height(x->right));

    // printf("\nAfter x Updation Right of y : %d", node_height(y));
    // printf("\nAfter x Updation Left of y : %d", node_height(y));


    y->node_height = 1 + max_val(node_height(y->left), node_height(y->right));

    // printf("\nAfter left rotation height : %d", node_height(y));
    return y;
}

Node * insert_node(Node * root, long int key, void * container){
    if(root == NULL){
        return create_Node(key, container);
    }

    if(key < root->key){
        root->left = insert_node(root->left, key, container);
    }
    else if(key > root->key){
        root->right = insert_node(root->right, key, container);
    }
    else{
        return root;
    }

    
    root->node_height = 1 + max_val(node_height(root->left), node_height(root->right));
    // printf("\nNew node height : %d", root->node_height);

    int balance = get_node_balance(root);

    if(balance > 1 && key < root->left->key){
        Node * temp =  rotate_node_right(root);
        // printf("\nNew node height after balancing : %d", root->node_height);
        return temp;
    }

    if(balance < -1 && key > root->right->key){
        Node * temp =  rotate_node_left(root);
        // printf("\nNew node height after balancing : %d", root->node_height);
        return temp;
    }

    if(balance > 1 && key > root->left->key){
        root->left = rotate_node_left(root->left);
        Node * temp =  root;
        // printf("\nNew node height after balancing : %d", root->node_height);
        return temp;
    }

    if(balance < -1 && key < root->right->key){
        root->right = rotate_node_right(root->right);
        Node * temp =  root;
        // printf("\nNew node height after balancing : %d", root->node_height);
        return temp;
    }

    return root;
}

Node * delete_node(Node * root, long int key){
    if(root == NULL)
        return root;

    if(key < root->key){
        root->left = delete_node(root->left, key);
    }
    else if(key > root->key){
        root->right = delete_node(root->right, key);
    }

    else {
        if(root->left == NULL || root->right == NULL){
            Node * t = root->left ? root->left : root->right ;

            if(t == NULL){
                t = root;
                root = NULL;
            }
            else{
                *root = *t;
            }
            if(t->container != NULL){
                free(t->container);
            }
            free(t);
        }
        else{
            Node * t = minimum_node(root->right);
            root->key = t->key;
            root->container = t->container;

            root->right = delete_node(root->right, t->key);
        }
    }

    if(root == NULL){
        return root;
    }

    root->node_height = 1 + max_val(node_height(root->left) , node_height(root->right));

    int balance = get_node_balance(root);

    if(balance > 1 && get_node_balance(root->left) >= 0){
        return rotate_node_right(root);
    }
    
    if(balance > 1 && get_node_balance(root->left) < 0){
        root->left = rotate_node_left(root->left);
        return rotate_node_right(root);
    }

    if(balance < -1 && get_node_balance(root->right) <= 0){
        return rotate_node_left(root);
    }
    if(balance < -1 && get_node_balance(root->right) > 0){
        root->right = rotate_node_right(root->right);
        return rotate_node_left(root);
    }

    return root;
}


void print_key_sequence(Node * root){
    if(root == NULL)
        return ;
    print_key_sequence(root->left);
    printf("%ld-%d ", root->key, node_height(root));
    print_key_sequence(root->right);
}

void print_all_leaf(Node * root){
    if(root == NULL)
        return ;

    print_all_leaf(root->left);
    print_all_leaf(root->right);

    if(root->left == NULL && root->right == NULL)
        printf("%ld ", root->key);
    return ;
}

Node * delete_tree(Node * root){
    if(root == NULL){
        return NULL;
    }

    root->left = delete_tree(root->left);
    root->right = delete_tree(root->right);

    // printf("\nDeleting key : %d", root->key);
    free(root->container);
    free(root);
    return NULL;
}

Node * find_node(Node * root, long int key){
    if(root == NULL){
        debug_log("\nReturning NULL");
        FLUSH();
        return NULL;
    }
    if(root->key < key){
        debug_log("\nGoing left");
        FLUSH();
        return find_node(root->left, key);
    }
    else if(root->key > key){
        debug_log("\nGoing right");
        FLUSH();
        return find_node(root->right, key);
    }
    else{
        debug_log("\nReturning right value");
        FLUSH();
        return root;
    }
    
    return root;
}