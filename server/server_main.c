#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "common_utils/sysadmin.h"
#include "accepter.h"
#include "server_main.h"

struct user user_list[USER_MAX];

ssize_t parse_user(char *line, struct user *dest) {
        dest->id = strtok(line, " ");
        dest->passwd = strtok(NULL, "\n");
        if (!(dest->id) || !(dest->passwd))
                return 0;

        return strlen(dest->passwd) + (dest->passwd - line) + 1;
}

int load_users() {
        struct stat s;
        if (stat(USER_INFO_FILE, &s) == -1)
                return -1;

        FILE *f = fopen(USER_INFO_FILE, "r");
        if (!f)
                return -1;

        char *buf = calloc(1, s.st_size + 1);
        if (fread(buf, s.st_size, 1, f) < 1)
                return -1;

        int i = 0;
        char *cur_line = buf;
        char *buf_end = buf + s.st_size;
        while (cur_line < buf_end) {
                ssize_t read = parse_user(cur_line, user_list + i);
                if (read == 0)
                        return -1;
                cur_line += read;
                i++;
                if (i > USER_MAX)
                        return -1;
        }

        return 1;
}

int main(int argc, char **argv)
{
        if (load_users() == -1)
                error_exit("Error loading user file");
        char *port_str = (argc > 1) ? argv[1] : NULL;
        int sockfd = make_socket(port_str, 1);
        serve(sockfd);
        return 0;
}
