#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "repl.h"
#include "command_handlers.h"
#include "common_utils/readwrite.h"
#include "common_utils/protocol_utils.h"

/*
 * Functions that handle specific commands
 * The spec is not specific on the status field of some command.  Here I assume
 * 0x01 in those cases.
 */

int handler_mola(struct state *mystate, char *arg);

struct cmd_info cmd_list[] = {
        {"open", BABY, handler_open, 2},
        {"auth", OPENED, handler_auth, 2},  /* TODO: change it to OPENED */
        {"quit", ANY, handler_exit, 0},
        {"mola", ANY, handler_mola, 1},
};

struct cmd_info *get_cmd_info(char *cmd_name)
{
        for (int i=0; i < sizeof(cmd_list)/sizeof(struct cmd_info); i++) {
                if (strcmp(cmd_name, cmd_list[i].name) == 0)
                        return &cmd_list[i];
        }

        return NULL;
}

/* for debug only */
int handler_mola(struct state *mystate, char *arg)
{
        swrite(mystate->sockfd, arg, 8);
        char *buf = calloc(9, sizeof(char));
        sread(mystate->sockfd, buf, 8);
        puts(buf);
        free(buf);
        return 1;
}

int handler_open(struct state *mystate, char *arg)
{
        puts("connecting...");

        struct sockaddr_in dest_addr = {
                .sin_family = AF_INET,
        };

        if (!inet_aton(strtok(arg, " "), &(dest_addr.sin_addr))) {
                fputs("invalid ip", stderr);
                return 0;
        }

        char * dest_port = strtok(NULL, " ");
        if (!dest_port) {
                fputs("Error: Server port not given", stderr);
                return 0;
        }
        /* TODO: validate port */
        dest_addr.sin_port = htons(atoi(dest_port));

        if (connect(mystate->sockfd, (struct sockaddr *)&dest_addr,
            sizeof(struct sockaddr)) == -1) {
                perror("Error establishing connection");
                return 0;
        }

        /* TODO: send OPEN_CONN_REQUEST */
        mystate->status = OPENED;
        return 1;
}

int handler_auth(struct state *mystate, char *arg)
{
        struct message_s recv_msg;
        write_head(mystate->sockfd, TYPE_AUTH, 0x01, strlen(arg));
        swrite(mystate->sockfd, arg, strlen(arg));
        read_head(mystate->sockfd, &recv_msg);
        if (recv_msg.status == 1) {
                puts("auth ok");
                mystate->status = AUTHED;
                return 1;
        } else if (recv_msg.status == 0) {
                puts("auth fail");
                mystate->status = BABY;
                return 0;
        } else {
                fputs("unknown reply from server", stderr);
                return 0;
        }
}

int handler_exit(struct state *mystate, char *argv)
{
        /* TODO: close socket */
        puts("bye");
        exit(0);
}
