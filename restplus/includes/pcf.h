/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#pragma once
//Do a platform check for all POSIX (Linux, MacOS)
#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <string.h>
typedef int SOCKET;
#define SOMAXCONN 128
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#else
#include <windows.h>
//Forward declaration of SOCKET
//It is defined in winsock2.h imported in socketfunc.hpp
typedef UINT_PTR SOCKET;
#endif
#include "nlohmann/json.hpp"
//Define json as nlohmann::json
using restjson = nlohmann::json;
