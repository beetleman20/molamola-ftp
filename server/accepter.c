#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "accepter.h"
#include "dedicated.h"

void mkthread_serve(int sockfd)
{
        /* TODO: multithreading */
        dedicated_serve(sockfd);
}

void accept_loop(int sockfd)
{
        struct sockaddr_in addr;
        socklen_t sin_size = sizeof(struct sockaddr_in);

        while (1) {
                int spawned_sockfd = accept(sockfd, (struct sockaddr *)&addr, &sin_size);
                if (spawned_sockfd == -1) {
                        perror("Error accept client");
                        continue;
                }

                puts("received connection");
                mkthread_serve(spawned_sockfd);
        }
}

void serve(int sockfd)
{
        if (listen(sockfd, 0) &&
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0
            ) {
                perror("Cannot listen");
                exit(1);
        }

        accept_loop(sockfd);
}
