#ifndef PROTOCOL_UTILS_H
#define PROTOCOL_UTILS_H

#define TYPE_OPEN_REQ 0xA1
#define TYPE_OPEN_REP 0xA2
#define TYPE_AUTH 0xA3

extern char fingerprint[];

struct message_s {
        char protocol[6];
        char type;
        char status;
        int length;
}  __attribute__ ((packed));

int write_head(int sockfd, char type, char status, int length);
int read_head(int sockfd, struct message_s *msg);;

#endif
