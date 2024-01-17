#pragma once
#ifdef __unix__
#include <unistd.h>
#include <string.h>
using SOCKET = int;
#define SOMAXCONN 128
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#else
#include <windows.h>
//Forward declaration of SOCKET
//It is defined in winsock2.h imported in socketfunc.hpp
typedef UINT_PTR SOCKET;
#endif