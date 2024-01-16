#include <string>
#include <map>

struct HTTPRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    void addHeader(std::string key, std::string value) {
        headers[key] = value;
    }
    void removeHeader(std::string key) {
        headers.erase(key);
    }
    void setBody(std::string body) {
        this->body = body;
    }
    void setMethod(std::string method) {
        this->method = method;
    }
    void setPath(std::string path) {
        this->path = path;
    }
    void setVersion(std::string version) {
        this->version = version;
    }
};

struct HTTPResponse {
    int response_code;
    std::string version;
    std::string status;
    std::map<std::string, std::string> headers;
    std::string body;
    void addHeader(std::string key, std::string value) {
        headers[key] = value;
    }
    void removeHeader(std::string key) {
        headers.erase(key);
    }
    void setBody(std::string body) {
        this->body = body;
    }
    void setResponseCode(int response_code) {
        this->response_code = response_code;
    }
    void setVersion(std::string version) {
        this->version = version;
    }
    std::string to_string();
};
