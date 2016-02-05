#include <stdio.h>
#include <string.h>
#include "repl.h"
#include "command_handlers.h"

#define CMD_BUF_SIZE 128

void print_prompt()
{
        printf(">>> ");  /* make it look like irb :-P */
}

/*
 * extract command name and arg.  If no arg, store NULL
 */
void get_cmd_arg(char **cmdname, char **arg, char *line)
{
        *cmdname = line;
        int len = strlen(line);
        *(line+len-1) = '\0';  /* strip newline */
        len--;
        for (; len > 0; len--) {
                if (*++line == ' ') {
                        *line = '\0';
                        while (*++line == ' ') ;
                        *arg = line;
                        return;
                }
        }

        *arg = NULL;
}

void command_loop(int sockfd)
{
        char cmd_buf[CMD_BUF_SIZE];
        struct state mystate = {"", 0};

        while (1) {
                print_prompt();

                char *ret = fgets(cmd_buf, sizeof(cmd_buf), stdin);
                if (!ret) {
                        /* ctrl-D pressed */
                        puts("ctrl-d!");
                        handler_exit(sockfd, NULL);
                }
                if (strcmp(cmd_buf, "\n") == 0)
                        continue;

                char *cmdname, *arg;
                get_cmd_arg(&cmdname, &arg, cmd_buf);
                //if (arg)
                struct cmd_info *ci = get_cmd_info(cmdname);
                if (!ci) {
                        printf("invalid command %s\n", cmdname);
                        continue;
                }

                if (!arg && ci->argc > 0) {
                        printf("command \"%s\" require argument\n", cmdname);
                        continue;
                }

                ci->handler(sockfd, arg);
        }
}
