#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "protocol_utils.h"
#include "readwrite.h"

char fingerprint[] = "\xe3myftp";

int write_head(int sockfd, unsigned char type,
               unsigned char status, uint32_t payload_length)
{
        struct message_s msg;
        memcpy(msg.protocol, fingerprint, sizeof(msg.protocol));
        msg.type = type;
        msg.length = htonl(payload_length + sizeof(msg));
        msg.status = status;
        return swrite(sockfd, &msg, sizeof(msg));
}

int read_head(int sockfd, struct message_s *msg)
{
        int ret;
        if ((ret = sread(sockfd, msg, sizeof(struct message_s))) == -1) {
                return -1;
        } else {
                msg->length = ntohl(msg->length);
                return ret;
        }
}

off_t payload_size(struct message_s *msg)
{
        return msg->length - sizeof(struct message_s);
}
