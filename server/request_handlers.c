#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "request_handlers.h"
#include "common_utils/readwrite.h"
#include "common_utils/protocol_utils.h"
#include "server_main.h"

/*
 * Functions that handle specific commands
 * The spec is not specific on the status field of some command.  Here I assume
 * 0x01 in those cases.
 */

int req_auth(int sockfd, struct message_s *msg);

struct req_info {
        char type_code;
        req_handler handler;
};

struct req_info req_list[] = {
        {TYPE_AUTH, req_auth}
};

req_handler get_handler(char type_code)
{
        for (int i=0; i < sizeof(req_list)/sizeof(struct req_info); i++) {
                if (req_list[i].type_code == type_code)
                        return req_list[i].handler;
        }

        return NULL;
}

char *payload_malloc(int sockfd, struct message_s *msg)
{
        /*
         * load a payload from socket.
         * WARNING: Don't use this function to read ~MB from socket
         * because it will out of memory.  Use the sendfile() system call.
         * sendfile() is a library function in sparc machine.
         */
        ssize_t len = msg->length - sizeof(struct message_s);
        char *payload = malloc(len);
        sread(sockfd, payload, len);
        return payload;
}

int req_auth(int sockfd, struct message_s *msg)
{
        char *payload = payload_malloc(sockfd, msg);
        printf("trying to authenicate with %s\n", payload);

        struct user guest;
        if (!parse_user(payload, &guest))
                return -1;

        int res = -1;
        for (int i=0; i < USER_MAX; i++) {
                if (!user_list[i].id) {
                        /* reach the end */
                        res = -1;
                        break;
                }
                if (strcmp(user_list[i].id, guest.id) == 0 &&
                   (strcmp(user_list[i].passwd, guest.passwd) == 0)) {
                        /* match */
                        printf("%s logged in\n", guest.id);
                        res = 0;
                        break;
                }
        }
        free(payload);

        if (res == 0)
                write_head(sockfd, 0xA2, 1, 0);
        else
                write_head(sockfd, 0xA2, 0, 0);

        return res;
}
