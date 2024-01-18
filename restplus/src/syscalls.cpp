/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#include "syscalls.hpp"
#include <iostream>
#ifdef __unix__
#include <limits.h>
#include <sys/stat.h>
#endif

void printlasterror() {
#ifdef __unix__
    std::cerr << strerror(errno) << "\n";
#else
    std::cerr << WSAGetLastError() << "\n";
#endif
}

void printerror(const char * error) {
    std::cerr << error << "\n";
}

bool is_file(const char * path) {
#ifdef __unix__
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
#else
    LPCWSTR fpath = (LPCWSTR) path;
    DWORD dwAttrib = GetFileAttributes(fpath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#endif
}