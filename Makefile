# Makefile for compiler

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic

EXE= go_compiler
TARGET= lexer
SRC=$(wildcard *.c)
HDR=$(wildcard *.h)
OBJ=$(SRC:%.c=%.o)

all: $(TARGET)-test #$(EXE)

# $(EXE): $(OBJ)
# 	$(CC) $(CFLAGS) -o $@ $^

$(TARGET)-test: $(TARGET)-test.o $(TARGET).o 
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
	rm -rf $(TARGET)-test