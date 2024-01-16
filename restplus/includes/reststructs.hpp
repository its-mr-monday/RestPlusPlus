#include <string>
#include <map>

struct HTTPRequestParams {
    std::map<std::string, std::string> params;
    void add(std::string key, std::string value);
    void remove(std::string key);
    std::string get(std::string key);
    bool isEmpty();
    bool has(std::string key);
};

struct HTTPRequest {
    std::string method;
    std::string path;
    HTTPRequestParams params;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    void addHeader(std::string key, std::string value);
    void removeHeader(std::string key);
    void setBody(std::string body);
    void setMethod(std::string method);
    void setPath(std::string path);
    void setVersion(std::string version);
    std::string to_string();
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
