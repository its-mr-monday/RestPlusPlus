#include "reststructs.hpp"
#include "restplusexception.hpp"
#include <future>
#include <thread>
#include <iostream>
#include <vector>

/* RestPlus API class
 * Used to generate REST API endpoints and start the server
 * Pass in a secret key and then use the On method to create endpoints
 * Then call the Start method to start the server
 */
class RestPlus {
    public:
        RestPlus(std::string secret_key);
        ~RestPlus();
        //Assgign a static route to a handler function
        //Handler function takes in a HTTPRequest and returns a HTTPResponse
        void On(std::string path, std::vector<std::string> methods, HTTPResponse (*handler)(HTTPRequest));
        //Assign a statis route with a single method to a handler function
        //Handler function takes in a HTTPRequest and returns a HTTPResponse
        void On(std::string path, std::string method, HTTPResponse (*handler)(HTTPRequest));
        //Start the server
        void Start(int port, bool debug = false, bool logging = false);
        //Start the server with default port 5000
        void Start(bool debug = false, bool logging = false);
        //Set the 404 handler function
        void Set404Handler(HTTPResponse (*handler)(HTTPRequest));
    private:
        RestPlusAPIInfo api_info;
        std::string secret_key;
        std::map<std::string, HTTPResponse (*)(HTTPRequest)> routes;
        std::map<std::string, std::vector<std::string>> route_methods;
        std::map<std::string, bool> route_dynamic;
        std::map<std::string, HTTPRequestParamFields> route_param_fields;
        std::vector<std::thread> threads;
        std::vector<std::string> get_methods(std::string path);
        HTTPResponse handle_request(HTTPRequest request);
        HTTPResponse (*handler404) (HTTPRequest);
        HTTPResponse run404(HTTPRequest request);
};

bool is_file(std::string file_path);
HTTPResponse send_file(std::string file_path, std::string filename, HTTPRequest request);
HTTPResponse json_res(std::string json, int status_code, HTTPRequest request);
/*
For any given request we will store one function

*/