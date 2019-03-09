#include "rsocketcontainer.h"
#include <stdio.h>
#include <stdlib.h>



int main(){
    Node * root;
    root = insert_node(root, 1, NULL);
    root = insert_node(root, 2, NULL);
    root = insert_node(root, 3, NULL);

    // printf("\nTree Height : %d\n", node_height(root));

    // print_all_leaf(root);

    root = insert_node(root, 10, NULL);
    root = insert_node(root, 20, NULL);
    root = insert_node(root, 30, NULL);

    // printf("\nTree Height : %d\n", node_height(root));


    // print_all_leaf(root);


    root = insert_node(root, 100, NULL);
    root = insert_node(root, 200, NULL);
    root = insert_node(root, 300, NULL);

    // printf("\nTree Height : %d\n", node_height(root));


    // print_all_leaf(root);


    root = insert_node(root, 104, NULL);
    root = insert_node(root, 203, NULL);
    root = insert_node(root, 110, NULL);


    // printf("\nTree Height : %d\n", node_height(root));

    // print_all_leaf(root);
    print_key_sequence(root);


    root = delete_node(root, 104);
    root = delete_node(root, 110);

    // printf("\nTree Height : %d\n", node_height(root));
    
    
    // print_all_leaf(root);

    printf("\n");

    print_key_sequence(root);

    // delete_tree(root);

    printf("\n");

    root = delete_tree(root);

    print_key_sequence(root);

    printf("\n");

    return 0;
}