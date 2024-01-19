/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#pragma once
#include "reststructs.hpp"
#include "restplusexception.hpp"
#include "restplusthreadpool.hpp"

HTTPRequestParams parse_request_params(HTTPRequest request, HTTPRequestParams params);
HTTPRequest parse_request(std::string request);
void log_request(HTTPRequest request, HTTPResponse response, std::string log_file_path);
void debug_request(HTTPRequest request, HTTPResponse response);

/* RestPlus API class
 * Used to generate REST API endpoints and start the server
 * Pass in a secret key and then use the On method to create endpoints
 * Then call the Start method to start the server
 */
class RestPlus {
    public:
        //Constructor for RestPlus class
        RestPlus(std::string secret_key);
        //Destructor for RestPlus class
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
        //Function takes in a HTTPRequest and returns a HTTPResponse
        //Used to handle a request by the server will call a handler function
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

        //HTTP Methods
        std::vector<std::string> get_methods(std::string path);
        HTTPResponse (*handler404) (HTTPRequest);
        HTTPResponse run404(HTTPRequest request);
        
};

//Static handler functions

//This function creates a HTTPResponse object containing the filedata of a given file
HTTPResponse send_file(std::string file_path, std::string filename);
HTTPResponse send_file(std::string file_path, std::string filename, bool as_attachment);
//This function creates a HTTPResponse object containing json data and a status code
HTTPResponse json_res(std::string json, int status_code);
//This function creates a HTTPResponse object prompting the client to redirect to a given url
HTTPResponse redirect(std::string url, int status_code = 302);
//This function adds a cookie to a HTTPResponse object
void add_cookie(HTTPResponse &response, std::string name, std::string value, int max_age = 0, std::string path = "/", std::string domain = "", bool secure = false, bool http_only = false);
/*
For any given request we will store one function

*/