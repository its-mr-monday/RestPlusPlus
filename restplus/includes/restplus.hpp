#pragma once
#include "reststructs.hpp"
#include "restplusexception.hpp"
#include "restplusthreadpool.hpp"
/* RestPlus API class
 * Used to generate REST API endpoints and start the server
 * Pass in a secret key and then use the On method to create endpoints
 * Then call the Start method to start the server
 */

//Thread management function
//Takes a reference to the running variable, a reference to the threads vector, and a reference to the current threads variable
//If running is false, then the function will return and allow the RestPlus object to clear threads and destroy itself
void thread_closer(bool &running, std::vector<std::thread> &threads, int &CURRENT_THREADS);
HTTPRequest parse_request(std::string request);
void log_request(HTTPRequest request, HTTPResponse response, std::string log_file_path);
void debug_request(HTTPRequest request, HTTPResponse response);

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
        //Set the maximum number of threads
        void SetMaxThreads(int max_threads);
        HTTPResponse handle_request(HTTPRequest request);
    private:
    //Private variables
        //The API info for the RestPlus class
        RestPlusAPIInfo api_info;
        //The thread pool for the RestPlus class
        RestPlusThreadPool thread_pool;
        //The secret key for the RestPlus class
        std::string secret_key;
        //The running variable for the RestPlus class
        bool running = false;
        //The map of routes to handler functions
        std::map<std::string, HTTPResponse (*)(HTTPRequest)> routes;
        //The map of routes to methods
        std::map<std::string, std::vector<std::string>> route_methods;
        //The map of routes to whether or not they are dynamic
        std::map<std::string, bool> route_dynamic;
        //The map of routes to their param fields (only used in dynamic routes)
        std::map<std::string, HTTPRequestParamFields> route_param_fields;

        //Thread management
        //The vector of client threads
        std::vector<std::thread> threads;
        //The thread manager thread
        std::thread thread_manager;
        //The maximum number of threads
        int MAX_THREADS = 10; //Can be changed by user
        int CURRENT_THREADS = 0;

        //HTTP Methods
        std::vector<std::string> get_methods(std::string path);
        HTTPResponse (*handler404) (HTTPRequest);
        HTTPResponse run404(HTTPRequest request);
        
};

bool is_file(std::string file_path);
HTTPResponse send_file(std::string file_path, std::string filename, HTTPRequest request);
HTTPResponse json_res(std::string json, int status_code, HTTPRequest request);
/*
For any given request we will store one function

*/