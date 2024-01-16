#include "routes.hpp"
#include <time.h>

HTTPResponse hello_world(HTTPRequest request) {
    HTTPResponse response;
    response.response_code = 200;
    response.body = "Hello World!";
    return response;
}

HTTPResponse current_time(HTTPRequest request) {
    HTTPResponse response;
    response.response_code = 200;
    std::string time_str = std::to_string(time(NULL));
    response.body = "{\"time\": \"" + time_str + "\"}";
    return response;
}

HTTPResponse hello_name(HTTPRequest request) {
    HTTPResponse response;
    response.response_code = 200;
    response.body = "Hello " + request.params.get("name") + "!";
    return response;
}