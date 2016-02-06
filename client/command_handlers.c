#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "command_handlers.h"
#include "readwrite.h"

int handler_mola(int sockfd, char *arg);

struct cmd_info cmd_list[] = {
        {"open", false, handler_open, 2},
        {"auth", false, handler_auth, 2},
        {"quit", false, handler_exit, 0},
        {"mola", false, handler_mola, 1},
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
int handler_mola(int sockfd, char *arg)
{
        swrite(sockfd, arg, strlen(arg));
        char *buf = calloc(9, sizeof(char));
        sread(sockfd, buf, 8);
        puts(buf);
        free(buf);
        return 1;
}

int handler_open(int sockfd, char *arg)
{
        puts("connecting...");

        struct sockaddr_in dest_addr = {
                .sin_family = AF_INET,
        };

        if (!inet_aton(strtok(arg, " "), &(dest_addr.sin_addr))) {
                puts("invalid ip");
                return 0;
        }

        char * dest_port = strtok(NULL, " ");
        if (!dest_port) {
                puts("Error: Server port not given");
                return 0;
        }
        /* TODO: validate port */
        dest_addr.sin_port = htons(atoi(dest_port));

        // don't forget to error check the connect()!
        if (connect(sockfd, (struct sockaddr *)&dest_addr,
            sizeof(struct sockaddr)) == -1) {
                perror("Error establishing connection");
                return 0;
        }
        return 1;
}

int handler_auth(int sockfd, char *arg)
{
        return 0;
}

int handler_exit(int sockfd, char *argv)
{
        /* TODO: close socket */
        puts("bye");
        exit(0);
}
