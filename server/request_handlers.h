#include <stdbool.h>
#include "common_utils/protocol_utils.h"

typedef int (*req_handler)(int sockfd, struct message_s *msg);

req_handler get_handler(char type_code);

int req_auth(int sockfd, struct message_s *msg);
int req_open(int sockfd, struct message_s *msg);
