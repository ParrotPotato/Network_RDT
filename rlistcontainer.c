#include "rlistcontainer.h"
#include <stdlib.h>
#include <stdio.h>

Node * create_Node(long int key, void * container){
    Node * ptr = (Node *)malloc(sizeof(Node));
    ptr->container = container;
    ptr->key = key ;
    ptr->next = NULL;
    return ptr;
}

Node * insert_node(Node * root, int key, void * container){
    Node * returnval = root;
    // printf("inserting value %d\n", key);
    if(root == NULL)
        return create_Node(key, container);

    while(root != NULL){
        if(root->next == NULL){
            root->next = create_Node(key, container);
            root->next->next = NULL;
            return returnval;
        }
        root = root->next;
    }
}

Node * delete_node(Node * root, int key){
    Node * returnval = root;
    if(root == NULL)
        return NULL;
    if(root->key == key){
        Node * ptr = root;
        if(root->container != NULL){
            free(root->container);
        }
        free(root);
        return root->next;
    }
    while(root != NULL){
        if(root->next == NULL)
            break;
        else if(root->next->key == key){
            Node * ptr = root->next;
            root->next = ptr->next;
            free(ptr);
            break;
        }
        root = root->next;
    }
    return returnval;
}


Node * find_node(Node * root, int key){
    if(root == NULL)
        return NULL;
    
    while(root != NULL){
        if(root->key == key)
            return root;
        else 
            root = root->next;
    }
    return NULL;
}

void print_list(Node * root){
    while(root!=NULL){
        printf("%ld ", root->key);
        root = root->next;
    }
}

Node * delete_list(Node * root){
    if(root == NULL){
        return NULL;
    }
    delete_list(root->next);
    free(root);
    return NULL;
}

void * apply_to_all_nodes(Node * root, void function_ptr(Node * ptr, void * args), void * args){
    while(root != NULL){
        function_ptr(root, args);
        root = root->next;
    }
}

int length(Node * root){
    int length_val = 0 ;
    while(root != NULL){
        root = root->next;
        length_val++;
    }
    return length_val;
}