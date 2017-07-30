#include <iterator>
#include <algorithm>
#include <map>
#include <string>
#include "catch.hpp"

#include "Response.h"
#include "Utilities.h"

using namespace Http;

TEST_CASE("Enum StatusCode", "[Response]")
{
    Response res;

    SECTION("enum to int")
    {
        REQUIRE(etoint(Response::StatusCode::OK) == 200);
        REQUIRE(etoint(Response::StatusCode::Bad_Request) == 400);
        REQUIRE(Response::status_code(Response::StatusCode::Found) == 302);
    }

    SECTION("enum to reason phrase")
    {
        const char *OK = Response::reason_phrase(Response::StatusCode::OK);
        REQUIRE(OK == "OK");

        std::string Bad_Request = Response::reason_phrase(Response::StatusCode::Bad_Request);
        REQUIRE(Bad_Request == "Bad Request");
    }
}
