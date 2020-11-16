# Makefile for compiler

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic

# all: compiler

# compiler : parser.o parser-test.o error.o lexer.o dynamicstring.o symtable.o
# 	$(CC) $(CFLAGS) -o $@ $^

all: lexer-test dynamicstring-test symtable-test parser-test codegenerator-test

compiler-test: 
	@bash ./tests/run_tests.sh

parser-test: parser.o parser-test.o error.o lexer.o dynamicstring.o symtable.o codegenerator.o
	$(CC) $(CFLAGS) -o $@ $^

lexer-test: lexer.o lexer-test.o dynamicstring.o error.o symtable.o
	$(CC) $(CFLAGS) -o $@ $^

dynamicstring-test: dynamicstring-test.o dynamicstring.o error.o
	$(CC) $(CFLAGS) -o $@ $^

symtable-test: symtable-test.o symtable.o error.o
	$(CC) $(CFLAGS) -o $@ $^

codegenerator-test: dynamicstring.o codegenerator-test.o codegenerator.o error.o
	$(CC) $(CFLAGS) -o $@ $^

code-test: codegenerator-test
	./codegenerator-test >out
	./ic20int out

pars-test: parser-test
	./parser-test <test_file.go
# No need
# lexer.o: lexer.c lexer.h dynamicstring.h error.h

# lexer-test.o: lexer.h lexer-test.c

# parser.o: parser.c parser.h lexer.h error.h

# parser-test.o: parser.h parser-test.c

# dynamicstring.o: dynamicstring.c dynamicstring.h error.h

# dynamicstring-test.o: dynamicstring.h dynamicstring-test.c

# symtable-test.o: symtable.h symtable-test.c

# error.o: error.c error.h

# symtable.o: symtable.c symtable.h error.h

clean:
	rm -rf *.o
	rm -rf lexer-test dynamicstring-test symtable-test parser-test codegenerator-test

zip:
	rm -rf *.o
	rm -rf compiler
	zip xrucek00.zip *.c *.h Makefile
