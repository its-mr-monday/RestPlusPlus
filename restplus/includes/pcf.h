#pragma once
#ifdef __unix__
#include <unistd.h>
#include <string.h>
using SOCKET = int;
#else
#include <windows.h>
typedef UINT_PTR SOCKET;
#endif