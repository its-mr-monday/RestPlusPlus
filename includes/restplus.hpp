#include "reststructs.hpp"
#include "restplusexception.hpp"
#include <thread>
#include <iostream>
#include <vector>

struct RestPlusAPIInfo {
    bool DEBUG_MODE = false;
    bool LOGGING = false;
    std::string LOG_FILE_NAME = "restplus.log";
    int PORT = 5000;
};

/* RestPlus API class
 * Used to generate REST API endpoints and start the server
 * Pass in a secret key and then use the On method to create endpoints
 * Then call the Start method to start the server
 */
class RestPlus {
    public:
        RestPlus(std::string secret_key);
        ~RestPlus();
        void On(std::string path, std::vector<std::string> methods, HTTPResponse (*handler)(HTTPRequest));
        void On(std::string path, std::string method, HTTPResponse (*handler)(HTTPRequest));
        void Start(int port, bool debug = false, bool logging = false);
        void Start(bool debug = false, bool logging = false);
    private:
        RestPlusAPIInfo api_info;
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