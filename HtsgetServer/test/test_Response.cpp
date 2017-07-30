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
    REQUIRE(res.reason_ == "");

    SECTION("enum to int")
    {
        res.status_code_ = Response::StatusCode::OK;
        int OK = etoint(res.status_code_);
        REQUIRE(OK == 200);

        res.status_code_ = Response::StatusCode::Bad_Request;
        int BAD_REQUEST = etoint(res.status_code_);
        REQUIRE(BAD_REQUEST == 400);

        /* Compile time evaluation */
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
