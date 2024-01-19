/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

#include "restplus.hpp"
#include "restplusthreadpool.hpp"
#include <sstream>
#include "socketfunc.hpp"
#include <fstream>
#include <regex>

struct ValidateDynamicRouteResponse {
    bool is_dynamic;
    HTTPRequestParamFields fields;
    HTTPRequestParams params;
};

bool fits_pattern(std::string route, std::string route_pattern) {
    //Route could be /home/zack/info
    //route_pattern could be /home/<name>/info
    //Another example could be: /api/<version>/get/<id>
    //And a passed route could be: /api/v1/get/1
    //Last example: /api/redirect/<url>
    //Last example: /api/redirect/https%3A%2F%2Fgoogle.com
}

std::string url_decode(std::string url) {

}

ValidateDynamicRouteResponse validate_and_match(std::string route,
    std::vector<std::string> dynamic_routes, std::map<std::string, 
    HTTPRequestParamFields> dynamic_route_fields) {
    ValidateDynamicRouteResponse response;
    response.is_dynamic = false;

}

HTTPRequestParams parse_request_params(HTTPRequest request, HTTPRequestParamFields fields) {
    HTTPRequestParams params;
    //remove first / from path
    std::string path = request.path.substr(1);
    std::stringstream ss(path);
    std::string section;
    int section_num = 0;
    bool lastRun = false;
    while (std::getline(ss, section, '/')) {
        //If we are at the last section_num append all the rest of the path to the last section
        if (section_num == fields.sections - 1) {
            lastRun = true;
            std::string last_section;
            while (std::getline(ss, last_section, '/')) {
                section += "/" + last_section;
            }
        }
        if (fields.section_defs[section_num].constant) {
            if (fields.section_defs[section_num].codename != section) {
                throw RestPlusException("Invalid path");
            }
        }
        else {
            params.add(fields.section_defs[section_num].codename, section);
        }
        if (lastRun) {
            break;
        }
    }
    return params;
}

HTTPRequest parse_request(std::string request) {
    HTTPRequest req;
    //Create a empty HTTPRequestParams object
    //A class function will be used to fill this object
    req.params = HTTPRequestParams();
    std::stringstream ss(request);
    std::string line;
    std::getline(ss, line);
    std::stringstream first_line(line);
    std::string method;
    std::string path;
    std::string version;
    first_line >> method >> path >> version;
    req.setMethod(method);
    req.setPath(path);
    req.setVersion(version);
    while (std::getline(ss, line)) {
        if (line == "\r") {
            break;
        }
        std::stringstream header_line(line);
        std::string key;
        std::string value;
        std::getline(header_line, key, ':');
        std::getline(header_line, value);
        req.addHeader(key, value);
    }
    std::string body;
    //Get the rest of the request as the body
    std::getline(ss, body, '\0');
    req.setBody(body);
    return req;
}

void log_request(HTTPRequest request, HTTPResponse response, std::string log_file_path) {
    const char* app_dir = get_appdir();
    const char* fpathPTR = path_join(app_dir, log_file_path.c_str());
    std::string fpath = std::string(fpathPTR);
    std::ofstream log_file;
    std::stringstream ss;
    ss << "HTTP" << request.method << " " << request.path << "" << response.response_code;
    ss << "\nBODY\n" << request.body << "\n";
    ss << "\nRESPONSE\n" << response.body << "\n"; 
    log_file.open(fpath, std::ios_base::app);
    log_file << ss.str() << std::endl;
    ss.clear();
    log_file.close();
}

void debug_request(HTTPRequest request, HTTPResponse response) {
    std::stringstream ss;
    //Simpler function
    //Sample print: HTTP GET /home 200
    ss << "HTTP" << request.method << " " << request.path << "" << response.response_code;
    std::cout << ss.str() << std::endl;
}

RestPlus::RestPlus(std::string secret_key) {
    this->secret_key = secret_key;
}

void RestPlus::SetMaxThreads(int max_threads) {
    this->thread_pool.set_max_threads(max_threads);
}

void RestPlus::Start(int port, bool debug = false, bool logging = false) {
    this->api_info.PORT = port;
    this->api_info.DEBUG_MODE = debug;
    this->api_info.LOGGING = logging;
    this->running = true;
    
    //Start the thread pool
    this->thread_pool.start();
    SERVER_SOCKET server_socket = create_server_socket("127.0.0.1", port);
    while (true) {
        try {
        //Accept as many connections as possible
            listen(server_socket.socket, SOMAXCONN);
            //Create a socket to hold the connection
            SOCKET client_socket;
            //Create a sockaddr_in object called client_address
            struct sockaddr_in client_address;
            socklen_t client_length = sizeof(client_address);
            //Create a thread to handle each connection
            while (true) {
                client_socket = accept(server_socket.socket, (struct sockaddr *) &client_address, &client_length);
                if (client_socket < 0 || client_socket == INVALID_SOCKET) {
                    std::stringstream ss;
                    ss << "Error on accept\n";
                    multiclose(client_socket);
                    throw RestPlusException(ss.str());
                }
                //Add this to the job
                this->thread_pool.newjob(client_socket, *this, this->api_info);
            }
        } catch (std::exception& e) {
            //If its a keyboard interrupt we must sigkill
            if (e.what() == "Keyboard Interrupt") {
                dispose_server_socket(server_socket);
                this->thread_pool.join();
                exit(0);
            }
            std::stringstream ss;
            ss << "Exception occured: " << e.what() << "\n";
            dispose_server_socket(server_socket);
            server_socket = create_server_socket("127.0.0.1", port);
        }
    }
    dispose_server_socket(server_socket);
    this->thread_pool.join();   //Kill all threads prior to exit
}

void RestPlus::Start(bool debug = false, bool logging = false) {
    this->Start(5000, debug, logging);
}

RestPlus::RestPlus(std::string secret_key) {
    this->secret_key = secret_key;
}

RestPlus::~RestPlus() {
    this->running = false;
    this->thread_pool.join();
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

bool is_dynamic(std::string path) {
    //use regex to determine if the path is dynamic
    std::regex pattern("<[a-zA-Z0-9]+>");
    std::smatch match;
    return std::regex_search(path, match, pattern);
}

HTTPRequestParamFields parse_fields(std::string path) {
    //Use regex to do this based on the path
    //Parse contants and
    HTTPRequestParamFields fields;
    //Example of a path: /home/<name>/info/<detail>
    //Example of a path: /home/<name>
    //Example of a path: /<filename>
    //Use these to parse the path this is used for the On method once the path is determined dynamic
    //We need to find the constant fields as well
    //We need to find the dynamic fields as well
    //We need to find the number of sections
    //We need to find the section definitions
    //We need to find the section definitions
    //Find the contant fields "fields without <>"
    //They should be started with a /

    std::stringstream ss(path);
    std::string section;
    int section_num = 0;
    while (std::getline(ss, section, '/')) {
        if (section == "") {
            continue;
        }
        if (section[0] == '<') {
            //This is a dynamic field
            //Remove the <> from the section
            section = section.substr(1, section.length() - 2);
            HTTPRequestParamField field;
            field.constant = false;
            field.codename = section;
            fields.section_defs[section_num] = field;
        }
        else {
            //This is a constant field
            HTTPRequestParamField field;
            field.constant = true;
            field.codename = section;
            fields.section_defs[section_num] = field;
        }
        section_num++;
    }
    fields.sections = section_num;
    fields.route = path;
    return fields;
}

void RestPlus::On(std::string path, std::vector<std::string> methods, HTTPResponse (*handler)(HTTPRequest)) {
    std::vector<std::string> existing_methods = this->get_methods(path);
    if (existing_methods.size() != 0) {
        for (auto& method : methods) {
            //Append any new methods to the existing methods
            for (auto& existing_method : existing_methods) {
                if (existing_method != method) {
                    existing_methods.push_back(method);
                }
            }
        }
    }
    //If the path is dynamic we must store the param fields
    bool dynamic = is_dynamic(path);
    if (dynamic) {
        HTTPRequestParamFields fields = parse_fields(path);
        this->route_param_fields[path] = fields;
        this->route_dynamic[path] = true;
    }
    //Check if there is a pattern in the path that makes it dynamic
    //<> is the pattern within this there should be a variable name

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
        } 
        
        else {
            res = this->run404(request);
            return res;
        }
    } catch (std::exception& e) {
        printerror(e.what());
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

//UTIL FUNCTION FOR send_file
std::string load_file_data(std::string file_path) {
    std::ifstream file(file_path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

HTTPResponse send_file(std::string file_path, std::string filename) {
    HTTPResponse res;
    if (file_path == "") {
        throw RestPlusException("File path cannot be empty");
    }
    if (!is_file(file_path.c_str())) {
        res.setBody("File not found");
        res.setResponseCode(404);
        res.addHeader("Content-Type", "text/plain");
        res.addHeader("Access-Control-Allow-Origin", "*");
        res.addHeader("Access-Control-Allow-Headers", "Content-Type");
        res.setVersion("HTTP/1.1");
        return res;
    }
    std::string file_data = load_file_data(file_path);
    res.setBody(file_data);
    res.setResponseCode(200);
    res.addHeader("Cache-Control", "no-cache");
    res.addHeader("Content-Disposition", "inline; filename=\"" + filename + "\"");
    res.addHeader("Content-Type", "application/octet-stream");
    res.addHeader("Access-Control-Allow-Origin", "*");
    res.addHeader("Access-Control-Allow-Headers", "Content-Type");
    res.setVersion("HTTP/1.1");
    return res;
}

HTTPResponse send_file(std::string file_path, std::string filename, bool as_attachment) {
    HTTPResponse baseResponse = send_file(file_path, filename);
    if (!as_attachment) {
        return baseResponse;
    }
    baseResponse.addHeader("Content-Disposition", "attachment; filename=\"" + filename + "\"");

    return baseResponse;
}

HTTPResponse json_res(std::string json, int status_code) {
    HTTPResponse res;
    res.setResponseCode(status_code);
    res.setBody(json);
    res.addHeader("Content-Type", "application/json");
    res.addHeader("Access-Control-Allow-Origin", "*");
    res.addHeader("Access-Control-Allow-Headers", "Content-Type");
    res.setVersion("HTTP/1.1");
    return res;
}

HTTPResponse redirect(std::string url, int status_code = 302) {
    HTTPResponse res;
    res.setResponseCode(status_code);
    res.setVersion("HTTP/1.1");

}

void add_cookie(HTTPResponse &response, std::string name, std::string value, int max_age = 0, std::string path = "/", std::string domain = "", bool secure = false, bool http_only = false) {
    std::stringstream ss;
    ss << name << "=" << value << ";";
    if (max_age != 0) {
        ss << "Max-Age=" << max_age << ";";
    }
    if (path != "") {
        ss << "Path=" << path << ";";
    }
    if (domain != "") {
        ss << "Domain=" << domain << ";";
    }
    if (secure) {
        ss << "Secure;";
    }
    if (http_only) {
        ss << "HttpOnly;";
    }
    response.addHeader("Set-Cookie", ss.str());
}