# IFJ Project

Implementation of IFJ20 compiler.
IFJ20 is a simplified version of the programming language Go.

## Authors

* Rebeka Cernianska, xcerni13
* Matej Jurik, xjurik12
* Marek Micek, xmicek08
* Peter Rucek, xrucek00

## Bugs

* Allocated memory for tokens of type STR or ID is never freed -  a good solution  
  should be freeing the mem when parser receives it and finishes it's work with it

* (MINOR) Common occurence of uninitialized variable usage (`valgrind --track-origins=yes ./lexer-test`)
  
____________

## Assignments

* xcerni13:
  * Study semantic analysis

* xjurik12:
  * Lexer module test (t.data.s)
  * Lexical errors & errors enum
  * Expression analysis

* xmicek08:
  * Study semantic analysis

* xrucek00:
  * Finish syntactic analysis

## Job Pool

* EMPTY
