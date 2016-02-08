#include <stdlib.h>
#include <stdio.h>
#include "request_handlers.h"
#include "readwrite.h"

void dedicated_serve(int sockfd)
{
        struct message_s head_recv;
        while (read_head(sockfd, &head_recv) != -1) {
                puts("read complete");
                req_handler handler = get_handler(head_recv.type);
                printf("Received request: %02x\n", head_recv.type);
                if (!handler) {
                        printf("Request %02x is malformmated\n", head_recv.type);
                }
                handler(sockfd, &head_recv);
        }
        /* reach head means client closed */
}

