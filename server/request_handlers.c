#include <stdlib.h>
#include <stdio.h>
#include "request_handlers.h"
#include "readwrite.h"
#include "protocol_utils.h"

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
        printf("length of payload is %d\n", len);
        char *payload = malloc(len);
        sread(sockfd, payload, len);
        return payload;
}

int req_auth(int sockfd, struct message_s *msg)
{
        puts("trying to authenicate");
        char *payload = payload_malloc(sockfd, msg);
        printf("%s is trying to authenicate\n", payload);
        free(payload);
        return 1;
}
