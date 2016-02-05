CC ?= gcc
CFLAGS = -std=c99 -Wall
CC_CMD = $(CC) $(CFLAGS) -o $@ -c $<

%.o: client/%.c
	$(CC_CMD)

%.o: server/%.c
	$(CC_CMD)

default: client_main

client_main: client_main.o repl.o command_handlers.o

.PHONY: clean

clean:
	rm *.o client_main
