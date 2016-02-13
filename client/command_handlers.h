#include "repl.h"

typedef int (*cmd_func)(struct state *mystate, char *argv);

struct cmd_info {
        char *name;
        enum status req_status;  /* require what status? */
        cmd_func handler;
        int argc;
};

extern struct cmd_info cmd_list[];

struct cmd_info *get_cmd_info(char *cmd_name);

int handler_open(struct state *mystate, char *arg);
int handler_auth(struct state *mystate, char *arg);
int handler_exit(struct state *mystate, char *arg);
