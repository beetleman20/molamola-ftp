#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command_handlers.h"

struct cmd_info cmd_list[] = {
        {"open", false, handler_auth, 2},
        {"quit", false, handler_exit, 0},
};

struct cmd_info *get_cmd_info(char *cmd_name)
{
        for (int i=0; i < sizeof(cmd_list)/sizeof(struct cmd_info); i++) {
                if (strcmp(cmd_name, cmd_list[i].name) == 0)
                        return &cmd_list[i];
        }

        return NULL;
}

int handler_auth(int sockfd, char *argv)
{
        puts("authenicating...");
        return 0;
}

int handler_exit(int sockfd, char *argv)
{
        /* TODO: close socket */
        puts("bye");
        exit(0);
}
