# Makefile for compiler

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic

all: compiler

compiler : parser.o parser-test.o error.o lexer.o dynamicstring.o symtable.o codegenerator.o stack.o expressions.o
	$(CC) $(CFLAGS) -o $@ $^

# all: lexer-test dynamicstring-test symtable-test parser-test codegenerator-test expressions-test

# compiler-test:
# 	@bash ./tests/run_tests.sh

# compiler-test-debug:
# 	@bash ./tests/run_tests.sh --dbg

# parser-test: parser.o parser-test.o error.o lexer.o dynamicstring.o symtable.o codegenerator.o stack.o expressions.o
# 	$(CC) $(CFLAGS) -o $@ $^

# lexer-test: lexer.o lexer-test.o dynamicstring.o error.o symtable.o
# 	$(CC) $(CFLAGS) -o $@ $^

# dynamicstring-test: dynamicstring-test.o dynamicstring.o error.o
# 	$(CC) $(CFLAGS) -o $@ $^

# symtable-test: symtable-test.o symtable.o error.o
# 	$(CC) $(CFLAGS) -o $@ $^

# codegenerator-test: dynamicstring.o codegenerator-test.o codegenerator.o error.o stack.o
# 	$(CC) $(CFLAGS) -o $@ $^

# expressions-test: parser.o expressions-test.o expressions.o dynamicstring.o stack.o error.o lexer.o symtable.o codegenerator.o
# 	$(CC) $(CFLAGS) -o $@ $^

# code-test: codegenerator-test
# 	./codegenerator-test >out
# 	./ic20int out

test: compiler
	./compiler <test_files/1.go >out
	./ic20int out
	./compiler <test_files/2.go >out
	./ic20int out
	./compiler <test_files/3.go >out
	./ic20int out
#	./compiler <test_files/4.go >out // prekryvanie
#	./ic20int out
	./compiler <test_files/5.go >out
	./ic20int out
	./compiler <test_files/6.go >out
	./ic20int out
	./compiler <test_files/7.go >out
	./ic20int out
#	./compiler <test_files/8.go >out // prekryvanie
#	./ic20int out
#	./compiler <test_files/9.go >out // delenie 0 riesit ?
#	./ic20int out
#	./compiler <test_files/10.go >out //kopu chyb
#	./ic20int out
	./compiler <test_files/example1.go >out
	./ic20int out
	./compiler <test_files/example2.go >out
	./ic20int out
	./compiler <test_files/example3.go >out
	./ic20int out
#	./compiler <test_files/two-pass.go >out // see file
#	./ic20int out
	./compiler <test_files/varfun.go >out
	./ic20int out

clean:
	rm -rf *.o
	rm -rf lexer-test dynamicstring-test symtable-test parser-test codegenerator-test expressions-test

zip:
	rm -rf *.o
	rm -rf compiler
	zip xrucek00.zip *.c *.h Makefile rozdeleni
