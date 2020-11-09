/**
 * @file symtable.h
 * @authors Marek Micek, ...
 * @date 23 OCT 2020
 * @brief Interface for the table of symbols
 */

#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "error.h"

#define MAX_RET_VAL 100
#define MAX_ARG 100

typedef enum 
{
    T_UNDEFINED,
    T_INT,
    T_FLOAT64,
    T_STRING,
    T_NIL,
    //T_BOOL  bonusove riesenie??? 

} Data_type;


typedef struct tData
{
    Data_type type;
    bool defined;
    bool is_var;
    bool is_function;           
    bool in_block;                  //< whether we are in block
    unsigned param_counter;         //< num of arguments
    unsigned ret_counter;           //< num of ret values
    int retval_arr[MAX_RET_VAL];    //< stores data types of ret values
    int arg_arr[MAX_ARG];           //< stores data types of arg values
    int line;                       //< line of symbol

} TData;


typedef struct tnode
{
    char *key;    //< key of node, same as ID
    TData data;
    struct tnode *lptr;
    struct tnode *rptr;

} TNode;


TNode* init_symtable(TNode *root);
TNode* search_symtable(TNode *root, char *k);
TNode* rewrite_data(TNode *node, TData new_data);  //< rewrite old data in node with new one
TNode* create_node(TData d, char *k);
TNode* insert_symtable(TNode *root, TData d, char *k);
void delete_symtable(TNode *root);

/* funkcie na implemetaciu vymazania jedneho uzla, mozno nepotrebne*/

TNode* most_left_node(TNode *root);     //< returns the most left node suitable for delete
TNode* delete_node(TNode *root, char *k); //< deletes the node with key k

#endif