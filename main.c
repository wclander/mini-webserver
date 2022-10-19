#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "logging.h"
#include "safesockets.h"


/*
Sends an http response to the socket connfd, with the given 
status code and message body

The created response is an HTTP/1.1 response with the Connection 
header set to close
*/
void http_response(int connfd, int status, char *content, size_t content_length) {
    char *status_message = NULL;
    switch (status) {
        case 200:
            status_message = "OK";
            break;
        case 413:
            status_message = "Headers too long";
            break;
        default:
            status_message = "Status Message";
            break;
    }
    int headers_length = strlen(status_message) 
                                        + (int) ceil(log10((double) content_length)) // for the contentlength header
                                        + 74;
    char *response = calloc(headers_length + content_length, sizeof(char));
    sprintf(response, "HTTP/1.1 %i %s\nContent-Type: text/html\nContent-Length: %i\nConnection: close\n\n", status, status_message, (int) content_length);
    memcpy(response + headers_length, content, content_length);

    write(connfd, response, headers_length + content_length);

    free(response);
} 

int main() {
    int port = 3000;
    int max_connections = 1;
    const size_t BUF_LENGTH = 512;
    const size_t MAX_CONTENT = 2048;
    int sock; 

    set_log_file(NULL);
 
    atexit(close_log_file);

    sock = safe_create_socket();

    void close_sock() {
        // atexit calls run in reverse order, so we can still use debug_log
        debug_log("Quitting...");
        close(sock);
    }

    atexit(close_sock);
    
    signal(SIGINT, exit);

    safe_bind_port(sock, port);

    safe_listen(sock, max_connections);

    debug_log("Listening on port %d", port);

    pid_t thread_pid;
    int children = 0;

    while(++children < max_connections && (thread_pid = fork())) {
        if (thread_pid == -1) {
            switch (errno) {
                case EAGAIN:
                    debug_log("Could not create threads, limit reached");
                    break; // continue with less threads or single-threaded
                case ENOMEM:
                    debug_log("Out of memory");
                    break;
                case ENOSYS:
                    debug_log("System does not support multi-threading");
                    break;
                default:
                    debug_log("Unexpected error occured creating threads: %d", errno);
                    exit(errno);
            }
        }
    }

    while(1) {
        struct sockaddr_in cliaddr;
        socklen_t cliaddrlen = sizeof(cliaddr);    
        int connfd = accept(sock, (struct sockaddr *) &cliaddr, &cliaddrlen);
        if (connfd == -1) {
            // handle accept errors
            continue;
        }

        char buf[BUF_LENGTH + 1];

        ssize_t rcount = read(connfd, buf, BUF_LENGTH);

        if (rcount == -1) {
            // handle read errors
            continue;
        } else if (rcount == 0) {
            // nothing read
            close(connfd);
            continue;
        }

        if (rcount > BUF_LENGTH) {
            char *header_error = "<h1>413: Headers too long</h1><p>Max header length 512bytes</p>";
            http_response(connfd, 413, header_error, strlen(header_error));
        } else if (rcount < 4 || strncmp(buf, "GET ", 4) != 0) {
            char *header_error = "<h1>405: Invalid HTTP method</h1><p>This server only accepts GET requests</p>";
            http_response(connfd, 405, header_error, strlen(header_error)); 
        } else {
            buf[BUF_LENGTH] = 0; // null terminate the header so we can treat it like a string
            char *prefix = "./public";
            char *filename = malloc(sizeof(char) * (BUF_LENGTH + strlen(prefix))); // can store the entire request if neccessary (prevent overflow attacks)
            strcpy(filename + strlen(prefix), buf + 3 + (buf[4] == '/')); // put end of path into filename
            strcpy(filename, prefix);
            filename[strlen(prefix)] = '/';
            char *end_of_filename = filename;

            bool contains_parent = false;
            while(*end_of_filename != ' ' &&    // end of url
                    *end_of_filename != '?' &&  // end of filename and start of query string
                    *end_of_filename != '#' &&  // end of filename
                    *end_of_filename != '\0') {
                ++end_of_filename;
                if (*end_of_filename == '.' && *(end_of_filename + 1) == '.') {
                    contains_parent = true;
                } 
            }
            *end_of_filename = '\0';

            // Deal with remaining headers and queries

             if (contains_parent) {
                char *header_error = "<h1>403: Forbidden</h1><p>No</p>";
                http_response(connfd, 403, header_error, strlen(header_error));
            } else {
                debug_log("Opening %s", filename);
                FILE *fp = fopen(filename, "r");
                if (fp) {
                    char content[MAX_CONTENT];
                    int read_length = fread(content,sizeof(char),  MAX_CONTENT, fp);
                    if(ferror(fp)) {
                        debug_log("File read error in file %s", filename);
                        char *header_error = "<h1>500: Internal Server Error</h1>";
                        http_response(connfd, 500, header_error, strlen(header_error));
                        clearerr(fp);
                    } else {
                        http_response(connfd, 200, content, read_length);
                    }

                    fclose(fp);
                } else {
                    char *header_error;
                    switch (errno) {
                        case EACCES:
                            debug_log("File doesn't exist or file permission error");
                            header_error = "<h1>404: not found</h1>";
                            http_response(connfd, 404, header_error, strlen(header_error));
                            break;
                        case ENOMEM:
                            debug_log("Out of memory");
                            header_error = "<h1>500: Internal Server Error</h1>";
                            http_response(connfd, 500, header_error, strlen(header_error));
                            break;
                        default:
                            debug_log("Error opening file: ", filename);
                            header_error = "<h1>500: Internal Server Error</h1>";
                            http_response(connfd, 500, header_error, strlen(header_error));
                    }
                }
            }

            free(filename);
        }

        close(connfd);   
    }

    return 0;
}
