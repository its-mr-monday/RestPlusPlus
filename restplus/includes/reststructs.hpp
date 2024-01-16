#include <string>
#include <map>

struct RestPlusAPIInfo {
    bool DEBUG_MODE = false;
    bool LOGGING = false;
    std::string LOG_FILE_NAME = "restplus.log";
    int PORT = 5000;
};


struct HTTPRequestParamField {
    bool constant = true;
    std::string codename;

};
/* 
Example route: /hello/<name>/info/<age>
int sections = 4;
section_defs = {
    {
        0: { contant: true, codename: "hello" }
    },
    {
        1: { contant: false, codename: "name" }
    },
    {
        2: { contant: true, codename: "info" }
    },
    {
        3: { contant: false, codename: "age" }
    }
}
*/
struct HTTPRequestParamFields {
    std::string route;
    int sections;
    std::map<int, HTTPRequestParamField> section_defs;

};

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
