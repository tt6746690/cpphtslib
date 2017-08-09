#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include "Uri.h"

using namespace Http;

TEST_CASE("uri encoding/decoding", "[Uri]")
{

    SECTION("ctohex")
    {
        REQUIRE(ctohex('#') == "23");
        REQUIRE(ctohex('%') == "25");
    }

    SECTION("encoding")
    {
        std::string url = u8"François";
        REQUIRE(Uri::urlencode(url) == "Fran%C3%A7ois");
    }
}