CC = gcc
CFLAGS = -std=gnu99 -Wall -I. -g
CC_CMD = $(CC) $(CFLAGS) -o $@ -c $<

%.o: client/%.c
	$(CC_CMD)

%.o: server/%.c
	$(CC_CMD)

%.o: common_utils/%.c
	$(CC_CMD)

default: client_main server_main

client_main: sysadmin.o protocol_utils.o readwrite.o client_main.o repl.o command_handlers.o

server_main: sysadmin.o protocol_utils.o readwrite.o accepter.o request_handlers.o

.PHONY: clean

clean:
	rm *.o client_main server_main
