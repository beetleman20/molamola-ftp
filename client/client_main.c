#include <stdlib.h>
#include "repl.h"
#include "common_utils/make_socket.h"

int main(int argc, char **argv)
{
        char *port_str = (argc > 1) ? argv[1] : NULL;
        int sockfd = make_socket(port_str);
        command_loop(sockfd);
        return 0;
}
