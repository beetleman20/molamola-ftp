struct state {
        char server_ip[16];
        int server_port;
};

void command_loop(int sockfd);
