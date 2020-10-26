/**
 * @file parser.h
 * @authors Peter Rucek ...
 * @date 25 Oct 2020
 * @brief Interface for the parser
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"

/*
    program -> "package main" ->|<---------------
    ^^^^^^^                     |               |
                                ----- func --------> EOF
*/
void program(FILE *f);

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
bool func(FILE *f);

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

void functions(FILE *f);

bool func_header(FILE *f);

void prolog(FILE *f);

bool expect_token(FILE *f, Token_type t_type, Keyword k);

void statement();

void definition_s();

void if_s();

void assignment_s();

void for_s();

void return_s();

#endif  