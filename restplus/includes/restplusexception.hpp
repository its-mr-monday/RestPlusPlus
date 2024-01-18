/*
    Copyright 2024 by its-mr-monday.
    All rights reserved
    This file is part of the RestPlusPlus Framework, and is released 
    under the "MIT License Agreement". Please see the LICENSE file that 
    should have been included as part of this package
*/

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