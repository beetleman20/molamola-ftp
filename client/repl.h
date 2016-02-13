#ifndef REPL_H
#define REPL_H

enum status { BABY, OPENED, AUTHED, ANY };

struct state {
        char server_ip[16];
        int server_port;
        int sockfd;
        enum status status;
};

void command_loop(int sockfd);

#endif
