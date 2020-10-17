/**
 * @file pareser.h
 * @authors Peter Rucek ...
 * @date 15 Oct 2020
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
void program();

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
void func();

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
void statement();

#endif  