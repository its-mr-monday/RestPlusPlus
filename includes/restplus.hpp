#include "reststructs.hpp"
#include "restplusexception.hpp"
#include <thread>
#include <iostream>
#include <vector>

class RestPlus {
    public:
        RestPlus(std::string secret_key);
        ~RestPlus();
        void On(std::string path, std::vector<std::string> methods, HTTPResponse (*handler)(HTTPRequest));
        void On(std::string path, std::string method, HTTPResponse (*handler)(HTTPRequest));
        void Start(int port);
    private:
        void listen(int port);
        std::string secret_key;
        std::map<std::string, HTTPResponse (*)(HTTPRequest)> routes;
        std::map<std::string, std::vector<std::string>> route_methods;
        std::vector<std::thread> threads;
        std::vector<std::string> get_methods(std::string path);
        HTTPResponse handle_request(HTTPRequest request);
};

/*
For any given request we will store one function

*/