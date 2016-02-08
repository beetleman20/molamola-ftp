#include <string.h>
#include "protocol_utils.h"
#include "readwrite.h"

char fingerprint[] = "\xe3myftp";

int write_head(int sockfd, char type, char status, int payload_length)
{
        struct message_s msg;
        memcpy(msg.protocol, fingerprint, sizeof(msg.protocol));
        msg.type = type;
        msg.length = payload_length + sizeof(msg);
        msg.status = status;
        return swrite(sockfd, &msg, sizeof(msg));
}

int read_head(int sockfd, struct message_s *msg)
{
        return sread(sockfd, msg, sizeof(struct message_s));
}
