#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "safesockets.h"

int safe_create_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0); // socket to listen on (IPV4, TCP, None)
    if (fd == -1) {
        switch errno {
            case EACCES:
                debug_log("Socket Permission denied");
                exit(errno);
            case EINVAL:
            case EAFNOSUPPORT:
            case EPROTONOSUPPORT:
                debug_log("Invalid protocol or address family for socket");
                exit(errno);
            case EMFILE:
            case ENFILE:
                debug_log("Socket limit reached");
                exit(errno);
            case ENOBUFS:
            case ENOMEM:
                debug_log("Out of memory");
                exit(errno);
            default:
                debug_log("Unexpected error creating socket: %d", errno);
                exit(errno);
        }
    }
    return fd;
}

void safe_bind_port(int sock, int port_number) {
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);
    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        switch (errno) {
            case EACCES:
                debug_log("Binding Permission denied");
                exit(errno);
            case EADDRINUSE:
                debug_log("Port in use: %d", port_number);
                exit(errno);
            case EBADF:
            case ENOTSOCK:
                debug_log("Invalid Socket");
                exit(errno);
            case EINVAL:
                debug_log("Socket already bound or invalid address");
                exit(errno);
            default:
                debug_log("Unexpected Error occured");
                exit(errno);
        }
    }
    return;
}

void safe_listen(int sock, int max_connections) {
    if (listen(sock, max_connections) < 0) {
        switch (errno) {
            case EADDRINUSE:
                debug_log("Cannot listen, port in use");
                exit(errno);
            case EBADF:
            case ENOTSOCK:
            case EOPNOTSUPP:
                debug_log("Invalid Socket");
                exit(errno);
            default:
                debug_log("Unexpected Error occured");
                exit(errno);
        }
    }
    return;
}
