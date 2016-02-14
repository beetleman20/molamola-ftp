/*
 * Read and write to socket safely.
 * Lots of the code is consulted from vsftp and yafc
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include "readwrite.h"

/*
 * guarantee len bytes read from fd
 */
int swrite(int fd, void *buf, unsigned int len)
{
        ssize_t ret;
        while (len > 0 && (ret = write(fd, buf, len)) != 0) {
                printf("somthing written %d\n", ret);
                /* if other side closed connection, -1 will be recieved
                 * with errno EPIPE */
                if (ret == -1) {
                        if (errno == EINTR)
                                continue;
                        perror("Error writing fd");
                        return -1;
                }
                buf += ret;
                len -= ret;
        }
        return 0;
}

/*
 * guarantee len bytes written to fd
 */
int sread(int fd, void *buf, unsigned int len)
{
        ssize_t ret;
        while (len > 0) {
                ret = read(fd, buf, len);
                printf("somthing read %d\n", ret);
                if (ret == -1) {
                        if (errno == EINTR)
                                continue;
                        perror("Error reading fd");
                        return -1;
                } else if (ret == 0) {
                        /* the other side closed connection */
                        return -1;
                }
                buf += ret;
                len -= ret;
        }
        return 0;
}

/*
 * copy file by the os kernel, in_fd must NOT be socket
 * this function is modified from "vsftpd, ftpdataio.c" 
 */
int transfer_file_sys(int out_fd, int in_fd, off_t num_send)
{
        while (num_send > 0) {
                unsigned int send_this_time;
                if (num_send > DATA_BUFSIZE) {
                        send_this_time = DATA_BUFSIZE;
                } else {
                        send_this_time = (unsigned int) num_send;
                }
                int ret = sendfile(out_fd, in_fd, NULL, send_this_time);
                if (ret == -1 || ret == 0)
                        return -1;
                num_send -= ret;
        }
        return 0;
}

/*
 * a slower version, but cross platform
 */
int transfer_file_copy(int out_fd, int in_fd, off_t num_send)
{
        void *buf = malloc(DATA_BUFSIZE);
        if (!buf) {
                perror("malloc error");
                exit(1);
        }
        while (num_send > 0) {
                unsigned int send_this_time;
                if (num_send > DATA_BUFSIZE) {
                        send_this_time = DATA_BUFSIZE;
                } else {
                        send_this_time = (unsigned int) num_send;
                }
                if (sread(in_fd, buf, send_this_time) == -1)
                        return -1;
                if (swrite(out_fd, buf, send_this_time) == -1)
                        return -1;
                num_send -= send_this_time;
        }
        return 0;
}
