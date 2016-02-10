#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "request_handlers.h"
#include "accepter.h"

void dedicated_serve(int sockfd)
{
        struct message_s head_recv;
        while (read_head(sockfd, &head_recv) != -1) {
                req_handler handler = get_handler(head_recv.type);
                printf("Received request: %02x\n", head_recv.type);
                if (!handler) {
                        fprintf(stderr, "Request %02x is malformmated\n", head_recv.type);
                }
                handler(sockfd, &head_recv);
        }
        /* reach head means client closed */
}

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
