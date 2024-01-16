#include "restplus.hpp"
#include <sstream>
#include "socketfunc.hpp"
#include <fstream>
#include <regex>
#ifdef __unix__
#include <limits.h>
#include <sys/stat.h>
#endif

bool is_file(std::string file_path) {
#ifdef __unix__
    struct stat buf;
    stat(file_path.c_str(), &buf);
    return S_ISREG(buf.st_mode);
#else
    LPCWSTR path = (LPCWSTR) file_path.c_str();
    DWORD dwAttrib = GetFileAttributes(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#endif
}

std::string get_application_dir() {
#ifdef __unix__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path = std::string(result, (count > 0) ? count : 0);
    std::size_t found = path.find_last_of("/\\");
    return path.substr(0, found);
#else
    LPWSTR path = new WCHAR[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    std::wstring ws(path);
    std::string str(ws.begin(), ws.end());
    std::size_t found = str.find_last_of("/\\");
    return str.substr(0, found);
#endif
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
    std::ofstream log_file;
    std::stringstream ss;
    ss << "HTTP" << request.method << " " << request.path << "" << response.response_code;
    ss << "\nBODY\n" << request.body << "\n";
    ss << "\nRESPONSE\n" << response.body << "\n"; 
    log_file.open(log_file_path, std::ios_base::app);
    log_file << ss.str() << std::endl;
    ss.clear();
    log_file.close();
}

RestPlus::RestPlus(std::string secret_key) {
    this->secret_key = secret_key;
}

void RestPlus::Start(int port, bool debug = false, bool logging = false) {
    this->api_info.PORT = port;
    this->api_info.DEBUG_MODE = debug;
    this->api_info.LOGGING = logging;
    SERVER_SOCKET server_socket = create_server_socket("127.0.0.1", port);
    while (true) {
    try {
#ifdef __unix__
    //Accept as many connections as possible
    listen(server_socket.socket, 5);
    //Create a socket to hold the connection
    SOCKET client_socket;
    //Create a sockaddr_in object called client_address
    struct sockaddr_in client_address;
    socklen_t client_length = sizeof(client_address);
    //Create a thread to handle each connection
    while (true) {
        client_socket = accept(server_socket.socket, (struct sockaddr *) &client_address, &client_length);
        if (client_socket < 0) {
            std::stringstream ss;
            ss << "Error on accept\n";
            throw RestPlusException(ss.str());
        }
        threads.push_back(std::thread([this, client_socket, client_address, client_length] {
            //Create a buffer to hold the incoming data
            char buffer[1024];
            //Read the data from the socket into the buffer
            int bytes_read = read(client_socket, buffer, 1024);
            if (bytes_read < 0) {
                std::stringstream ss;
                ss << "Error reading from socket\n";
                throw RestPlusException(ss.str());
            }
            //Parse the data into a HTTPRequest object
            HTTPRequest request = parse_request(buffer);
            //Handle the request
            HTTPResponse response = this->handle_request(request);
            //Send the response
            std::string response_string = response.to_string();
            int bytes_sent = send(client_socket, response_string.c_str(), response_string.length(), 0);
            if (bytes_sent < 0) {
                std::stringstream ss;
                ss << "Error sending response\n";
                throw RestPlusException(ss.str());
            }
            //Close the socket
            close(client_socket);
            //Log the request
            if (this->api_info.LOGGING) {
                std::string log_file_path = get_application_dir() + "/" + this->api_info.LOG_FILE_NAME;
                log_request(request, response, log_file_path);
            }
        }));
    }
#else
    //Listen for requests
    listen(server_socket.socket, SOMAXCONN);
    //Create a socket to hold the connection
    SOCKET client_socket;
    //Create a sockaddr_in object called client_address
    struct sockaddr_in client_address;
    int client_length = sizeof(client_address);
    //Create a thread to handle each connection
    while (true) {
        client_socket = accept(server_socket.socket, (struct sockaddr *) &client_address, &client_length);
        if (client_socket == INVALID_SOCKET) {
            std::stringstream ss;
            ss << "Error on accept\n";
            //close client socket and continue
            closesocket(client_socket);
            continue;
        }
        threads.push_back(std::thread([this, client_socket, client_address, client_length] {
            //Create a buffer to hold the incoming data
            char buffer[1024];
            //Read the data from the socket into the buffer
            int bytes_read = recv(client_socket, buffer, 1024, 0);
            if (bytes_read == SOCKET_ERROR) {
                std::stringstream ss;
                ss << "Error reading from socket\n";
                throw RestPlusException(ss.str());
            }
            //Parse the data into a HTTPRequest object
            HTTPRequest request = parse_request(buffer);
            //Handle the request
            HTTPResponse response = this->handle_request(request);
            //Send the response
            std::string response_string = response.to_string();
            int bytes_sent = send(client_socket, response_string.c_str(), response_string.length(), 0);
            if (bytes_sent == SOCKET_ERROR) {
                std::stringstream ss;
                ss << "Error sending response\n";
                closesocket(client_socket);

            }
            //Close the socket
            closesocket(client_socket);
            //Log the request
            if (this->api_info.LOGGING) {
                std::string log_file_path = get_application_dir() + "/" + this->api_info.LOG_FILE_NAME;
                log_request(request, response, log_file_path);
            }
        }));
    }

#endif
    } catch (std::exception& e) {
        //If its a keyboard interrupt we must sigkill
        if (e.what() == "Keyboard Interrupt") {
            dispose_server_socket(server_socket);
            exit(0);
        }
        std::stringstream ss;
        ss << "Exception occured: " << e.what() << "\n";
        dispose_server_socket(server_socket);
        server_socket = create_server_socket("127.0.0.1", port);
    }}
    dispose_server_socket(server_socket);
}

void RestPlus::Start(bool debug = false, bool logging = false) {
    this->Start(5000, debug, logging);
}

RestPlus::RestPlus(std::string secret_key) {
    this->secret_key = secret_key;
}

RestPlus::~RestPlus() {
    for (auto& thread : threads) {
        thread.join();
    }
}

void RestPlus::remove_route(std::string route) {
    this->routes.erase(route);
    this->route_methods.erase(route);
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
    HTTPRequestParamFields fields;
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

HTTPResponse send_file(std::string file_path, HTTPRequest request) {
    HTTPResponse res;
    if (file_path == "") {
        throw RestPlusException("File path cannot be empty");
    }
    if (!is_file(file_path)) {
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
    res.addHeader("Content-Type", "text/plain");
    res.addHeader("Access-Control-Allow-Origin", "*");
    res.addHeader("Access-Control-Allow-Headers", "Content-Type");
    res.setVersion("HTTP/1.1");
    return res;
}

HTTPResponse json_res(std::string json, int status_code, HTTPRequest request) {
    HTTPResponse res;
    res.setResponseCode(status_code);
    res.setBody(json);
    res.addHeader("Content-Type", "application/json");
    res.addHeader("Access-Control-Allow-Origin", "*");
    res.addHeader("Access-Control-Allow-Headers", "Content-Type");
    res.setVersion("HTTP/1.1");
    return res;
}