#include "routes.hpp"

int main() {
    RestPlus api("SECRET");
    api.On("/login", "POST", &login);

}