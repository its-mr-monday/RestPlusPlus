#include "routes.hpp"

int main() {
    RestPlus api("SECRET");
    api.On("/login", "POST", &login);
    api.On("/data", std::vector<std::string>({ "GET", "POST"}), &dataendpoint); 
}