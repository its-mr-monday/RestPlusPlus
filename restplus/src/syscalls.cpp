/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#include "syscalls.hpp"
#include <iostream>
#include <string>
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

const char * get_appdir() {
#ifdef __unix__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path = std::string(result, (count > 0) ? count : 0);
    std::size_t found = path.find_last_of("/\\");
    path.substr(0, found).c_str();
#else
    LPWSTR path = new WCHAR[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    std::wstring ws(path);
    std::string str(ws.begin(), ws.end());
    std::size_t found = str.find_last_of("/\\");
    return str.substr(0, found).c_str();
#endif
}

const char * path_join(const char * path, const char * path2) {
#ifdef __unix__
    std::string p1 = std::string(path);
    std::string p2 = std::string(path2);
    std::string p3 = p1 + "/" + p2;
    return p3.c_str();
#else
    std::string ws1(path);
    std::string ws2(path2);
    std::string ws3 = ws1 + "\\" + ws2;
    return ws3.c_str();
#endif
}