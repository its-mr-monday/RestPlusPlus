#include "reststructs.hpp"
#include <sstream>

std::string HTTPResponse::to_string() {
    std::stringstream ss;
    ss << this->version << " " << this->response_code << " " << this->status << "\n";
    for (auto const& x : this->headers) {
        ss << x.first << ": " << x.second << "\n";
    }
    ss << "\n" << this->body;
    return ss.str();
}