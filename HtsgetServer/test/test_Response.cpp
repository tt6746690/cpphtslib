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

    SECTION("StatusCode enum to int")
    {
        REQUIRE(Response::status_code(StatusCode::Found) == 302);
    }

    SECTION("StatusCode enum to reason phrase")
    {
        const char *OK = Response::reason_phrase(StatusCode::OK);
        REQUIRE(OK == "OK");

        std::string Bad_Request = Response::reason_phrase(StatusCode::Bad_Request);
        REQUIRE(Bad_Request == "Bad Request");
    }
}
