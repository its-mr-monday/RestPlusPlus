/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

//Defines sockets functions for posix and Windows
#pragma once
#include "pcf.h"
#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
//POSIX headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <shlwapi.h>
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

struct SERVER_SOCKET {
    SOCKET socket;
    sockaddr_in address;
};

struct CLIENT_SOCKET {
    SOCKET socket;
    sockaddr_in address;
};

//Cross platform function will create a SERVER_SOCKET object and return it
SERVER_SOCKET create_server_socket(const char * addr, int port);

//Cross platform function will take in a client SOCKET and close it
void multiclose(SOCKET socket);

//Cross platform function will take in a SERVER_SOCKET and close it
void dispose_server_socket(SERVER_SOCKET server_socket);

//Cross platform function will take in a SOCKET, a buffer, and a buffer size and read from the socket into the buffer
int multiread(SOCKET socket, char* buffer, int buffer_size);