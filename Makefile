CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRCS = main.c lexer.c parser.c semantic.c tac.c optimizer.c codegen.c
OBJS = $(SRCS:.c=.o)
EXEC = compiler.exe

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c compiler.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJS) $(EXEC)
