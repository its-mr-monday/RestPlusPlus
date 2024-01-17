#include "socketfunc.hpp"
#include "restplusexception.hpp"
#include <sstream>
#include <string>
#include <string.h>
#include <iostream>

SERVER_SOCKET create_server_socket(const char * addr, int port) {
    SERVER_SOCKET server_socket;
#ifdef __unix__
    //Create a socket for listening for incoming connections.
    server_socket.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket.socket < 0) {
        std::stringstream ss;
        ss << "Error opening socket\n";
        throw RestPlusException(ss.str());
    }
    //Clear the address struct
    sockaddr_in server_address;
    memset((char *) &server_address, 0, sizeof(server_address));

    //Set the address family
    server_socket.address.sin_family = AF_INET;
    //Set the address to listen on
    server_socket.address.sin_addr.s_addr = INADDR_ANY;
    //Set the port to listen on
    server_socket.address.sin_port = htons(port);
    //Bind the socket to the address
    if (bind(server_socket.socket, (struct sockaddr *) &server_socket.address, sizeof(server_socket.address)) < 0) {
        std::stringstream ss;
        ss << "Error on binding\n";
        throw RestPlusException(ss.str());
    }

#else
    //Create a WSADATA object called wsaData and also call WSAStartup.
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
        std::stringstream ss;
        ss << "Error at WSAStartup()\n";
        throw RestPlusException(ss.str());
    }
    //Create a SOCKET object called m_socket.
    server_socket.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket.socket == INVALID_SOCKET) {
        std::stringstream ss;
        ss << "Error at socket(): " << WSAGetLastError() << "\n";
        WSACleanup();
        throw RestPlusException(ss.str());
    }
    //Create a sockaddr_in object called service.
    server_socket.address.sin_family = AF_INET;
    server_socket.address.sin_addr.s_addr = inet_addr(addr);
    server_socket.address.sin_port = htons(port);
    //Call the bind function, passing the created socket and the sockaddr_in structure as parameters.
    //Check for general errors.
    if (bind(server_socket.socket, (SOCKADDR*)&server_socket.address, sizeof(server_socket.address)) == SOCKET_ERROR) {
        std::stringstream ss;
        ss << "bind() failed with error: " << WSAGetLastError() << "\n";
        closesocket(server_socket.socket);
        WSACleanup();
        throw RestPlusException(ss.str());
    }
    //Return the socket to listen for incoming connections.
#endif
    return server_socket;
}

void dispose_server_socket(SERVER_SOCKET server_socket) {
#ifdef __unix__
    close(server_socket.socket);
#else
    closesocket(server_socket.socket);
    WSACleanup();
#endif
}

void multiclose(SOCKET socket) {
#ifdef __unix__
    close(socket);
#else
    closesocket(socket);
#endif
}

int multiread(SOCKET socket, char* buffer, int buffer_size) {
#ifdef __unix__
    return read(socket, buffer, buffer_size);
#else
    return recv(socket, buffer, buffer_size, 0);
#endif
}