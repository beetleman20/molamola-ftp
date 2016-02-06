#include <stdlib.h>
#include <stdio.h>
#include "readwrite.h"

void dedicated_serve(int sockfd)
{
        ssize_t head_bufsize = 8;
        char *head_buf = malloc(head_bufsize);
        while (sread(sockfd, head_buf, head_bufsize) != -1) {
                puts("read complete");
                swrite(sockfd, "BJ! ", 4);
                swrite(sockfd, head_buf, 8);
        }
        free(head_buf);
}
