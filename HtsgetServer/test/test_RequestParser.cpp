#include <iterator>
#include <algorithm>
#include <map>
#include <string>
#include "catch.hpp"

#include "RequestParser.h"
#include "Request.h"
#include "Constants.h"

using namespace Http;

TEST_CASE("Request populated by parser properly", "[RequestParser]")
{
    RequestParser parser;
    REQUIRE(parser.state_ == RequestParser::State::req_start);

    Request req;
    REQUIRE(req.method_ == "");
    REQUIRE(req.uri_ == "");
    REQUIRE(req.headers_.size() == 0);
    REQUIRE(req.body_ == "");

    SECTION("ex1")
    {
        std::string payload = "GET /hi HTTP/1.0\r\n"
                              "Host: 127.0.0.1:8888\r\n"
                              "User-Agent: curl/7.43.0\r\n"
                              "Accept: */*\r\n"
                              "\r\n";

        parser.parse(req, std::begin(payload), std::end(payload));

        REQUIRE(req.method_ == "GET");
        REQUIRE(req.uri_ == "/hi");
        REQUIRE(req.version_major_ == 1);
        REQUIRE(req.version_minor_ == 0);
        REQUIRE(req.headers_.size() == 3);

        std::map<std::string, std::string> headers{
            {"Host", "127.0.0.1:8888"},
            {"User-Agent", "curl/7.43.0"},
            {"Accept", "*/*"}};

        for (auto header : headers)
        {
            auto found = find_if(
                req.headers_.begin(), req.headers_.end(),
                [header](auto built_header) {
                    return header.first == built_header.first;
                });
            REQUIRE(found != req.headers_.end());
            REQUIRE(found->first == header.first);
            REQUIRE(found->second == header.second);
        }
    }
}
