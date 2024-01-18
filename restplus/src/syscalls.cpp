/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#include "syscalls.hpp"
#include <iostream>

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