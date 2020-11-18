/**
 * @file parser.h
 * @authors Peter Rucek ...
 * @date 25 Oct 2020
 * @brief Interface for the parser
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "error.h"
#include "symtable.h"
#include "codegenerator.h"
#include "stack.h"
#include "expressions.h"

#define ARR_TREE_RANGE 1000 //< range of tree arrays
#define BUILT_FUNC_NUM 10   //< we have 10 built_in functions

typedef struct
{
    Token current_token;
    Token previous_token;
    int current_line;
    TNode *global_table;
    TNode *local_table;
    int index;          //< for searching of retval_arr and arg_arr
    
}Metadata;
 

TData new_data_func, new_data_var;

TData init_new_data(TData new_data);

void check_suspected();

void add_suspected_tree();

void delete_arr_suspected();

bool is_built_fun(char *func_id);

void check_built(TNode *root);

void program();

bool func();

void header_arg();

void header_ret();

bool func_header(bool *is_main);

void prolog();

bool expect_token(Token_type t_type, Keyword k);

bool statement();

void function_call(Token id, unsigned num_of_id, bool is_built);

void if_s();

void assignment_s(unsigned number_of_id);

void for_s();

void return_s();

bool expression();

bool input(int *input_token, bool *func_call);

#endif