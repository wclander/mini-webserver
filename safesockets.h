/* 
    Contains error-handling versions of the c socket, bind and listen functions  
*/

#ifndef SAFE_SOCKETS_H
#define SAFE_SOCKETS_H

#include "logging.h"

/*
Creates and returns a TCP socket

exits if the resulting socket is invalid
*/
int safe_create_socket();

/*
Binds the given socket to port_number

exits if binding fails
*/
void safe_bind_port(int sock, int port_number);


/*
Sets the give socket to listen

exits if the call to listen() fails
*/
void safe_listen(int sock, int max_connections);

#endif
