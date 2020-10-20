# Makefile for compiler

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic

UNAME_S := $(shell uname -s)

all: lexer-test dynamicstring-test

test: lexer-test

testrun:
ifeq ($(UNAME_S),Linux)
	@if setarch `uname -m` -R true 2>/dev/null; then setarch `uname -m` -R ./lexer-test; else ./lexer-test; fi
else
	./lexer-test
endif

lexer-test: lexer.o lexer-test.o dynamicstring.o error.o
	$(CC) $(CFLAGS) -o $@ $^

dynamicstring-test: dynamicstring-test.o dynamicstring.o
	$(CC) $(CFLAGS) -o $@ $^

lexer.o: lexer.c lexer.h 

lexer-test.o: lexer.h lexer-test.c

dynamicstring.o: dynamicstring.c dynamicstring.h

dynamicstring-test.o: dynamicstring.h dynamicstring-test.c

error.o: error.c error.h  

clean:
	rm -rf *.o
	rm -rf lexer-test dynamicstring-test