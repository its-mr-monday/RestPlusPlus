//Defines sockets functions for posix and Windows
#pragma once
#include "pcf.h"
#ifdef __unix__
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

SERVER_SOCKET create_server_socket(const char * addr, int port);

void dispose_server_socket(SERVER_SOCKET server_socket);