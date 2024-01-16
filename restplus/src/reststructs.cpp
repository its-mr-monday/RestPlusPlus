#include "reststructs.hpp"
#include <sstream>

void HTTPRequestParams::add(std::string key, std::string value) {
    params[key] = value;
}

void HTTPRequestParams::remove(std::string key) {
    params.erase(key);
}

std::string HTTPRequestParams::get(std::string key) {
    if (params.find(key) == params.end()) {
        return "";
    }
    return params[key];
}

bool HTTPRequestParams::isEmpty() {
    return params.empty();
}

bool HTTPRequestParams::has(std::string key) {
    return params.find(key) != params.end();
}

std::string HTTPResponse::to_string() {
    std::stringstream ss;
    ss << this->version << " " << this->response_code << " " << this->status << "\n";
    for (auto const& x : this->headers) {
        ss << x.first << ": " << x.second << "\n";
    }
    ss << "\n" << this->body;
    return ss.str();
}

void HTTPResponse::addHeader(std::string key, std::string value) {
    headers[key] = value;
}

void HTTPResponse::removeHeader(std::string key) {
    headers.erase(key);
}

void HTTPResponse::setBody(std::string body) {
    this->body = body;
}

void HTTPResponse::setResponseCode(int response_code) {
    this->response_code = response_code;
}

void HTTPResponse::setVersion(std::string version) {
    this->version = version;
}

void HTTPRequest::addHeader(std::string key, std::string value) {
    headers[key] = value;
}

void HTTPRequest::removeHeader(std::string key) {
    headers.erase(key);
}

void HTTPRequest::setBody(std::string body) {
    this->body = body;
}

void HTTPRequest::setMethod(std::string method) {
    this->method = method;
}

void HTTPRequest::setPath(std::string path) {
    this->path = path;
}

void HTTPRequest::setVersion(std::string version) {
    this->version = version;
}

std::string HTTPRequest::to_string() {
    std::stringstream ss;
    ss << this->method << " " << this->path << " " << this->version << "\n";
    for (auto const& x : this->headers) {
        ss << x.first << ": " << x.second << "\n";
    }
    ss << "\n" << this->body;
    return ss.str();
}

