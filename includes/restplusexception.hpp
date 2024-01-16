#pragma once
#include <string>

class RestPlusException : public std::exception {
    public:
        RestPlusException(std::string message) {
            this->message = message;
        }
        const char* what() const throw() {
            return message.c_str();
        }
    private:
        std::string message;
};