#include "routes.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

HTTPResponse login(HTTPRequest request) {
    std::string body = request.body;
    json j = json::parse(body);
    std::string username = j["username"];
    std::string password = j["password"];
    //Do some db call
    json response;
    HTTPResponse res;
    res.setResponseCode(200);
    res.setBody(response.dump());
    res.addHeader("Content-Type", "application/json");
    res.addHeader("Access-Control-Allow-Origin", "*");
    res.addHeader("Access-Control-Allow-Headers", "Content-Type");
    res.setVersion("HTTP/1.1");
    return res;
}

HTTPResponse dataendpoint(HTTPRequest request) {
    json response;
    if (request.method == "GET") {
        response["data"] = "some data";
        HTTPResponse res;
        res.setResponseCode(200);
        res.setBody(response.dump());
        res.addHeader("Content-Type", "application/json");
        res.addHeader("Access-Control-Allow-Origin", "*");
        res.addHeader("Access-Control-Allow-Headers", "Content-Type");
        res.setVersion("HTTP/1.1");
        return res;
    } else if (request.method == "POST") {
        json req = json::parse(request.body);
        std::string data = req["data"];
        //Do some db call
        HTTPResponse res;
        response["data"] = "some more data";
        res.setResponseCode(200);
        res.setBody(response.dump());
        res.addHeader("Content-Type", "application/json");
        res.addHeader("Access-Control-Allow-Origin", "*");
        res.addHeader("Access-Control-Allow-Headers", "Content-Type");
        res.setVersion("HTTP/1.1");
        return res;
    }
}