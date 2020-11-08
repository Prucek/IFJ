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


typedef struct
{
    Token actual_token;
    int actual_line;
    TNode *global_table;
    TNode *local_table;
    int index;          //< for searching of retval_arr and arg_arr
    
}Metadata;
/*
typedef struct Suspect
{
    struct Suspect *next;
    char *suspected;        //< stores functions suspected from no_definition
} *TSuspect;

typedef struct 
{
    TSuspect first;
} List;
*/

TData new_data_func, new_data_var;

TData init_new_data(TData new_data);

/*
    program -> "package main" ->|<---------------
    ^^^^^^^                     |               |
                                ----- func --------> EOF
*/
int program();

/*
    func -> "func" -> ID -> "(" -----------------> ")" --------------------------------------> "{" -> EOL -> statement -> "}"
    ^^^^                     |                      |      |                               |
                             |-----<- "," <---      |      |       //return values         |
                             |               |      |      |                               |
                             ---> ID -> type ------->      "("--------------------> ")"--->
                                                            |<-","  <----         |
                                //parameters                |           |         |
                                                            ---- type ------------>
    func factorial(n int) (int) {
        ...
    }
    func main() {
        ...
    }
*/
bool func();

/*
    statement --> EOL
    ^^^^^^^^^ |
              --> expression -> EOL
              |
   definiton  --> ID -> ":=" -> expression -> EOL
              |
       if     --> "if" -> expression -> "{" -> EOL -> statement -> "}" -> "else" -> "{" -> EOL -> statement -> "}" -> EOL
              |
   assignment -------> ID -> "=" -> expression -> EOL
              |
       for    --> "for" -----------------> ";" -> expression -> ";" -----------------> "{" -> statement -> "}" -> EOL
              |          |              |                           |              |
              |          -- definition ->                           -- assignment ->
              |
     return   --> "return" -> expression
*/

void header_arg();

void header_ret();

bool func_header();

void prolog();

bool expect_token(Token_type t_type, Keyword k);

bool statement();

void function_call();

void if_s();

void assignment_s(int number_of_id);

void for_s();

void return_s();

void expression();

#endif