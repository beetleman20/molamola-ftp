#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "repl.h"
#include "command_handlers.h"
#include "common_utils/readwrite.h"
#include "common_utils/protocol_utils.h"
#include "common_utils/make_socket.h"

/*
 * Functions that handle specific commands
 * The spec is not specific on the status field of some command.  Here I assume
 * 0x01 in those cases.
 */

int handler_mola(struct state *mystate, char *arg);
int handler_get(struct state *mystate, char *arg);

struct cmd_info cmd_list[] = {
        {"open", BABY, handler_open, 2},
        {"auth", OPENED, handler_auth, 2},  /* TODO: change it to OPENED */
        {"quit", ANY, handler_exit, 0},
        {"mola", ANY, handler_mola, 1},
        {"get", AUTHED, handler_get, 1},
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
        return 0;
}

int handler_open(struct state *mystate, char *arg)
{
        puts("connecting...");

        struct sockaddr_in dest_addr = {
                .sin_family = AF_INET,
        };

        if (!inet_aton(strtok(arg, " "), &(dest_addr.sin_addr))) {
                fputs("invalid ip", stderr);
                return -1;
        }

        char * dest_port = strtok(NULL, " ");
        if (!dest_port) {
                fputs("Error: Server port not given", stderr);
                return -1;
        }
        /* TODO: validate port */
        dest_addr.sin_port = htons(atoi(dest_port));

        int sockfd = make_socket(NULL, 0);
        if (connect(sockfd, (struct sockaddr *)&dest_addr,
            sizeof(struct sockaddr)) == -1) {
                perror("Error establishing connection");
                close(sockfd);
                return -1;
        }

        /* TODO: send OPEN_CONN_REQUEST */
        mystate->sockfd = sockfd;
        mystate->status = OPENED;
        return 0;
}

int handler_auth(struct state *mystate, char *arg)
{
        struct message_s recv_msg;
        write_head(mystate->sockfd, TYPE_AUTH, STATUS_UNUSED, strlen(arg));
        swrite(mystate->sockfd, arg, strlen(arg));
        read_head(mystate->sockfd, &recv_msg);
        if (recv_msg.status == 1) {
                puts("auth ok");
                mystate->status = AUTHED;
                return 0;
        } else if (recv_msg.status == 0) {
                puts("auth fail");
                mystate->status = BABY;
                return -1;
        } else {
                fputs("unknown reply from server", stderr);
                return -1;
        }
}

int handler_get(struct state *mystate, char *arg)
{
        struct message_s recv_msg;
        write_head(mystate->sockfd, TYPE_GET_REQ, STATUS_UNUSED, strlen(arg));
        swrite(mystate->sockfd, arg, strlen(arg));
        read_head(mystate->sockfd, &recv_msg);
        if (recv_msg.status == 1) {
                puts("file exists");
                return 0;
        } else if (recv_msg.status == 0) {
                puts("file not exists");
                return -1;
        } else {
                fputs("unknown reply from server", stderr);
                return -1;
        }
}

int handler_exit(struct state *mystate, char *argv)
{
        /* TODO: close socket */
        puts("bye");
        exit(0);
}
