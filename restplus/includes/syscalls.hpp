/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/
#pragma once
#include <stdio.h>
#ifdef __unix__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#else
#include <windows.h>
#endif

//This header file contains multi platform translated syscalls to be used
//Will reduce the amount of code needed to be written for multi platform support

//Prints the last error to stderr
//POSIX: Uses strerror(errno) to get last error
//Windows: Uses WSAGetLastError() to get last error
void printlasterror();

//Prints a string to stderr
void printerror(const char * error);

//Checks if a given path is a file
//Returns true if it is a file, false if it is not
bool is_file(const char * path);

//Get the applications ROOT directory
const char * get_appdir();

//Join a path with a second path
//Returns a new path seperated by "/" for POSIX and "\\" for Windows
const char * path_join(const char * path, const char * path2);