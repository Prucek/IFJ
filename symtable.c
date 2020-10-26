/**
 * @file symtable.c
 * @authors Marek Micek, ...
 * @date 24 OCT 2020
 * @brief Implementation of the table of symbols
 */

#include "symtable.h"


TNode* init_symtable(TNode *root)
{
    root = NULL;
    return root;
}


TNode* search_symtable(TNode *root, int k)
{
    if (root == NULL)
    {
        return NULL;
    }
    else if (root->key == k)
    {
        return root;
    }
    else if (root->key > k)
    {
        return (search_symtable(root->lptr, k));    //< searching left side
    }
    else
    {
        return (search_symtable(root->rptr, k));    //< searching right side
    }
}


TNode *rewrite_data(TNode *node, TData new_data)
{
    strcpy(node->data.id, new_data.id);
    node->data.type = new_data.type;
    node->data.defined = new_data.defined;
    node->data.global = new_data.global;
    node->data.is_function = new_data.is_function;
    node->data.param_counter = new_data.param_counter;

    return node;
}


TNode* create_node(TData d, int k)
{
    TNode *new_node = (TNode *) malloc(sizeof(TNode));
    if (new_node == NULL)
    {
        intern_error();
        return NULL;
    }

    new_node->data.id = (char *) malloc(sizeof(strlen(d.id) + 1));
    if (new_node->data.id == NULL)
    {
        intern_error();
        return NULL;
    }
    else
    {
        new_node->key = k;
        new_node->lptr = NULL;
        new_node->rptr = NULL;
        return (rewrite_data(new_node, d));
    }
}


TNode* insert_symtable(TNode *root, TData d, int k)
{
    if (root == NULL)   //< create new node cause key was not found
    {
         return create_node(d, k);
    }

    if (root->key > k)
    {
        root->lptr = insert_symtable(root->lptr, d, k);     //< trying insert to left child
    }
    else if (root->key < k)
    {
        root->rptr = insert_symtable(root->rptr, d, k);     //< trying insert to right child
    }
    else    //< key was found, rewrite data 
    {
        root->data.id = (char *) realloc(root->data.id, sizeof(strlen(d.id) + 1));
        if (root->data.id == NULL)
        {
            intern_error();
            return NULL;
        }
    }
    return (rewrite_data(root, d));
}


void delete_symtable(TNode *root)
{
    if (root != NULL)
    {
        delete_symtable(root->lptr);
        free(root->data.id);        //< must free the heap with id
        delete_symtable(root->rptr);
        free(root);
    }
}

/* funkcie na implemetaciu vymazania jedneho uzla, mozno nepotrebne */

TNode* most_left_node(TNode *root)
{
    if (root->lptr == NULL) //< the next left child does not exist
    {
        return root;
    }
    else
    {
        return (most_left_node(root->lptr));
    }  
}


TNode *delete_node(TNode *root, int k)
{
    if (root == NULL)
    {
        return NULL;
    }
    else if (root->key > k)     //< searching key is on the left
    {
        root->lptr = delete_node(root->lptr, k);
        return root;
    }
    else if (root->key < k)     //< searching key is on the right
    {
        root->rptr = delete_node(root->rptr, k);
        return root;
    }
    else                        //< key was found
    {
        if (root->lptr == NULL && root->rptr == NULL)   //< in case node has no children
        {
            free(root->data.id);    //< must free the heap with id
            free(root); 
            return NULL;
        }
        else if (root->lptr != NULL && root->rptr != NULL)  //< deleted node has 2 children
        {
            TNode *min = most_left_node(root->rptr);
            root->data.id = (char *) realloc(root->data.id, sizeof(strlen(min->data.id) + 1));
            if (root->data.id == NULL)
            {     
                intern_error();
                return NULL;
            }
            else
            {
                root->key = min->key;
                root = rewrite_data(root, min->data);
                root->rptr = delete_node(root->rptr, min->key);
                return root;
            }
        }
        else    //< deleted node has one child
        {
            TNode *only_child;
            if (root->lptr == NULL)     //< there is no left child
            {
                only_child = root->rptr;
            }
            else                        //< there is no right child
            {
                only_child = root->lptr;
            }
            free(root->data.id);    //< must free the heap with id
            free(root);
            return only_child;
        }
    } 
}

