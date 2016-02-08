/*
 * Read and write to socket safely.
 * Lots of the code is consulted from vsftp and yafc
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "readwrite.h"

/*
 * guarantee len bytes read from sockfd
 */
int swrite(int sockfd, void *buf, unsigned int len)
{
        ssize_t ret;
        while (len > 0 && (ret = write(sockfd, buf, len)) != 0) {
                printf("somthing written %d\n", ret);
                /* if other side closed connection, -1 will be recieved
                 * with errno EPIPE */
                if (ret == -1) {
                        if (errno == EINTR)
                                continue;
                        perror("Error writing socket");
                        return -1;
                }
                buf += ret;
                len -= ret;
        }
        return 1;
}

/*
 * guarantee len bytes written to sockfd
 */
int sread(int sockfd, void *buf, unsigned int len)
{
        ssize_t ret;
        while (len > 0 && (ret = read(sockfd, buf, len)) != 0) {
                printf("somthing read %d\n", ret);
                if (ret == -1) {
                        if (errno == EINTR)
                                continue;
                        perror("Error reading socket");
                        return -1;
                } else if (ret == 0) {
                        /* the other side closed connection */
                        return 0;
                }
                buf += ret;
                len -= ret;
        }
        return 1;
}
