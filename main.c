#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ERROR_HANDLING
#define DEBUG

void log(char *message, ...) {
    va_list ap;
#ifdef DEBUG
    vaprintf(stderr, message, ap);
#endif
}

int safe_create_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0); // socket to listen on (IPV4, TCP, None)
#ifdef ERROR_HANDLING
    if (fd = -1) {
        switch errno {
            case EACCES:
                log("Socket Permission denied");
                exit(errno);
            case EINVAL:
            case EAFNOSUPPORT:
            case EPROTONOSUPPORT:
                log("Invalid protocol or address family for socket");
                exit(errno);
            case EMFILE:
            case ENFILE:
                log("Socket limit reached");
                exit(errno);
            case ENOBUFS:
            case ENOMEM:
                log("Out of memory");
                exit(errno);
            default:
                log("Unexpected error creating socket: ", errno);
                exit(errno);
        }
    }
#endif
    return fd;
}

void safe_bind_port(int sock, int port_number) {
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);
    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
#ifdef ERROR_HANDLING
        switch (errno) {
            case EACCES:
                log("Binding Permission denied");
                exit(errno);
            case EADDRINUSE:
                log("Port in use: ", port_number);
                exit(errno);
            case EBADF:
            case ENOTSOCK:
                log("Invalid Socket");
                exit(errno);
            case EINVAL:
                log("Socket already bound or invalid address");
                exit(errno);
            default:
                log("Unexpected Error occured");
                exit(errno);
        }
#endif
    }
    return;
}

void safe_listen(int sock, int max_connections) {
    if (listen(sock, max_connections) < 0) {
#ifdef ERROR_HANDLING
        switch (errno) {
            case EADDRINUSE:
                log("Cannot listen, port in use");
                exit(errno);
            case EBADF:
            case ENOTSOCK:
            case EOPNOTSUPP:
                log("Invalid Socket");
                exit(errno);
            default:
                log("Unexpected Error occured");
                exit(errno);
        }
#endif
    }
    return;
}

void http_response(int connfd, int status, char *content, size_t content_length) {
    char *status_message = NULL;
    switch (status) {
        
    }
    size_t response_length = sizeof(char) * (content_length + strlen(status_message) + 
    char *response = malloc(
} 

int main() {
    int port = 80;
    int max_connections = 1;
    int sock = safe_create_socket();
    const size_t BUF_LENGTH = 512;
    
    safe_bind_port(sock, port);

    safe_listen(sock, max_connections);

    pid_t thread_pid;
    int children = 0;

    while(thread_pid = fork() && ++children < max_connetions) {
#ifdef ERROR_HANDLING
        switch (errno) {
            case EAGAIN:
                log("Could not create threads, limit reached");
                continue; // continue with less threads or single-threaded
            case ENOMEM:
                log("Out of memory");
                continue;
            case ENOSYS:
                log("System does not support multi-threading");
                continue;
            default:
                log("Unexpected error occured creating threads", errno);
                exit(errno);
        }
#endif
    }

    while(1) {
        struct sockaddr_in cliaddr;
        socklen_t cliaddrlen = sizeof(cliaddr);    
        int connfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (connfd == -1) {
            // handle accept errors
            continue;
        }

        char buf[BUF_LENGTH + 1];

        ssize_t rcount = read(connfd, buf, buffer_length);

        if (rcount == -1) {
            // handle read errors
            continue;
        } else if (rcount == 0) {
            // nothing read
            close(conffd);
            continue;
        }

        // For now ignore the actual request

        int response_code = 200;
        char *response = "HTTP/1.1" + ;
        write()

        close(connfd);   
    }

    return 0;
}
