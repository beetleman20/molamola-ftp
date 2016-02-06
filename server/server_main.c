#include <stdlib.h>
#include "make_socket.h"
#include "accepter.h"

int main(int argc, char **argv)
{
        char *port_str = (argc > 1) ? argv[1] : NULL;
        int sockfd = make_socket(port_str);
        serve(sockfd);
        return 0;
}
