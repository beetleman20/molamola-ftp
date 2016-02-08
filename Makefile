CC ?= gcc
CFLAGS = -std=gnu99 -Wall -I common_utils -g
CC_CMD = $(CC) $(CFLAGS) -o $@ -c $<

%.o: client/%.c
	$(CC_CMD)

%.o: server/%.c
	$(CC_CMD)

%.o: common_utils/%.c
	$(CC_CMD)

default: client_main server_main

client_main: make_socket.o protocol_utils.o readwrite.o client_main.o repl.o command_handlers.o

server_main: make_socket.o protocol_utils.o readwrite.o accepter.o dedicated.o request_handlers.o

accepter.o: readwrite.o dedicated.o

.PHONY: clean

clean:
	rm *.o client_main
