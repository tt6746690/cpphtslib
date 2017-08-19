#include "catch.hpp"
#include <utility>
#include <stdexcept>
#include <string>
#include <iostream>

#include "Encoding.h"

using namespace Http;


auto test_encoding(std::pair<std::string, std::string> in_out) -> void {

    auto in = std::get<0>(in_out);
    auto out = std::get<1>(in_out);

    auto input = byte_array(in.begin(), in.end());
    auto output = byte_array(out.begin(), out.end());

    auto input_encoded = base64_encode(input);
    REQUIRE(input_encoded == output);

    auto in_encoded = base64_encode(in);
    REQUIRE(in_encoded == output);
}


TEST_CASE("base64 encoding/decoding", "[Encoding]")
{

   
    SECTION("base64 encoding")
    {
        test_encoding({"pleasure.", "cGxlYXN1cmUu"});
        test_encoding({"leasure.", "bGVhc3VyZS4="});
        test_encoding({"easure.", "ZWFzdXJlLg=="});
        test_encoding({"asure.", "YXN1cmUu"});
        test_encoding({"sure.", "c3VyZS4="});
    }
}