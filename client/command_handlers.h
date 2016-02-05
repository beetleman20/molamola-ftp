#include <stdbool.h>

typedef int (*cmd_func)(int sockfd, char *argv);

struct cmd_info {
        char *name;
        bool req_auth;  /* require authenication? */
        cmd_func handler;
        int argc;
};

extern struct cmd_info cmd_list[];

struct cmd_info *get_cmd_info(char *cmd_name);

int handler_open(int sockfd, char *arg);
int handler_auth(int sockfd, char *arg);
int handler_exit(int sockfd, char *arg);
