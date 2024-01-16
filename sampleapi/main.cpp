#include "routes.hpp"

int main() {
    RestPlus api("SECRET_KEY");
    api.On("/hello", "GET", &hello_world);
    api.Start(5000, true, true);
}