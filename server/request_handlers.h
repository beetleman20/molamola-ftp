#include <stdbool.h>
#include "protocol_utils.h"

typedef int (*req_handler)(int sockfd, struct message_s *msg);

req_handler get_handler(char type_code);
