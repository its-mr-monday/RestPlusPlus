/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#pragma once
#include "syscalls.hpp"
#ifdef __unix__
typedef int SOCKET;
#define SOMAXCONN 128
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#else
//Forward declaration of SOCKET
//It is defined in winsock2.h imported in socketfunc.hpp
typedef UINT_PTR SOCKET;
#endif