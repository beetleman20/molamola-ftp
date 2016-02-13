#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
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
int req_get(int sockfd, struct message_s *msg);

struct req_info {
        char type_code;
        req_handler handler;
};

struct req_info req_list[] = {
        {TYPE_GET_REQ, req_get},
};

req_handler get_handler(char type_code)
{
        for (int i=0; i < sizeof(req_list)/sizeof(struct req_info); i++) {
                if (req_list[i].type_code == type_code)
                        return req_list[i].handler;
        }

        return NULL;
}

char *payload_malloc(int sockfd, struct message_s *msg, bool is_str)
{
        /*
         * load a payload from socket.
         * WARNING: Don't use this function to read ~MB from socket
         * because it will out of memory.  Use the sendfile() system call.
         * sendfile() is a library function in sparc machine.
         */
        ssize_t len = msg->length - sizeof(struct message_s);
        size_t buf_size = is_str ? len + 1 : len;
        char *payload = calloc(buf_size, 1);
        if (!payload) {
                perror("malloc error");
                exit(1);
        }
        sread(sockfd, payload, len);
        return payload;
}

int req_auth(int sockfd, struct message_s *msg)
{
        char *payload = payload_malloc(sockfd, msg, true);

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

int req_get(int sockfd, struct message_s *msg)
{
        char *filepath = payload_malloc(sockfd, msg, true);
        if (access(filepath, R_OK) != 0) {
                write_head(sockfd, TYPE_GET_REP, 0, 0);
                /* though not readable, marked as success because it is legal */
                return 0;
        }
        /* send GET_REPLY header */
        write_head(sockfd, TYPE_GET_REP, 1, 0);

        int local_fd = open(filepath, O_RDONLY);
        if (local_fd == -1) {
                /* maybe using up file descriptor, etc... */
                perror("error opening file");
                return -1;
        }

        struct stat st;
        stat(filepath, &st);
        /* send FILE_DATA header */
        write_head(sockfd, TYPE_FILE_DATA, STATUS_UNUSED, st.st_size);
        if (transfer_file_sys(sockfd, local_fd, st.st_size) == -1) {
                perror("error sending file");
                return -1;
        }

        close(local_fd);
        return 0;
}
