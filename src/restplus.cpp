#include "restplus.hpp"
#include <sstream>
#ifdef __unix__
//POSIX headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
//Windows headers
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <shlwapi.h>
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

RestPlus::RestPlus(std::string secret_key) {
    this->secret_key = secret_key;
}

RestPlus::~RestPlus() {
    for (auto& thread : threads) {
        thread.join();
    }
}

std::vector<std::string> RestPlus::get_methods(std::string path) {
    std::vector<std::string> methods;
    for (auto& route : route_methods) {
        if (route.first.find(path) != std::string::npos) {
            for (auto& method : route.second) {
                methods.push_back(method);
            }
        }
    }
    return methods;
}

void RestPlus::On(std::string path, std::vector<std::string> methods, HTTPResponse (*handler)(HTTPRequest)) {
    std::vector<std::string> existing_methods = this->get_methods(path);
    for (auto& method : methods) {
        //Append any new methods to the existing methods
        if (std::find(existing_methods.begin(), existing_methods.end(), method) == existing_methods.end()) {
            existing_methods.push_back(method);
        }
    }
    this->routes[path] = handler;
    this->route_methods[path] = existing_methods;
}

void RestPlus::On(std::string path, std::string method, HTTPResponse (*handler)(HTTPRequest)) {
    std::vector<std::string> methods;
    methods.push_back(method);
    this->On(path, methods, handler);
}

HTTPResponse RestPlus::handle_request(HTTPRequest request) {
    std::string key = request.method + request.path;
    HTTPResponse res;
    try {
        if (routes.find(key) != routes.end()) {
            res = routes[key](request);
        } else {
            res.setResponseCode(404);
            res.setBody("Not Found");
            res.addHeader("Content-Type", "text/plain");
            res.addHeader("Access-Control-Allow-Origin", "*");
            res.addHeader("Access-Control-Allow-Headers", "Content-Type");
            res.setVersion("HTTP/1.1");
            return res;
        }
    } catch (std::exception& e) {
        res.setResponseCode(500);
        res.setBody("Internal Server Error");
        res.addHeader("Content-Type", "text/plain");
        res.addHeader("Access-Control-Allow-Origin", "*");
        res.addHeader("Access-Control-Allow-Headers", "Content-Type");
        res.setVersion("HTTP/1.1");
        return res;
    }
    return res;
}

