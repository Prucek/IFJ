CC = gcc
CFLAGS = -std=c99 -g -Wall -Werror -Wextra -pedantic
BINARIES = test_lexer

all: $(BINARIES)

test_lexer: lexer_suite/test_lexer.o lexer_suite/error.o lexer_suite/lexer.o lexer_suite/dynamicstring.o
	$(CC) $(CFLAGS) -o $@ $?

lexer_suite/test_lexer.o: lexer_suite/test_lexer.c 
	$(CC) $(CFLAGS) -c -o $@ $?

lexer_suite/error.o: ../error.c
	$(CC) $(CFLAGS) -c -o $@ $?

lexer_suite/lexer.o: ../lexer.c
	$(CC) $(CFLAGS) -c -o $@ $?

lexer_suite/dynamicstring.o: ../dynamicstring.c
	$(CC) $(CFLAGS) -c -o $@ $?


clean:
	rm -f $(BINARIES)
	rm -f lexer_suite/*.o