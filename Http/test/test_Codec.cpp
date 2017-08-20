#include "catch.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "Codec.h"

using namespace Http;

auto test_codec(std::pair<std::string, std::string> in_out) -> void {

  auto in = std::get<0>(in_out);
  auto out = std::get<1>(in_out);

  auto input = BYTE_STRING(in.begin(), in.end());
  auto output = BYTE_STRING(out.begin(), out.end());

  // encoding
  auto encoded = Base64Codec::encode(input);
  REQUIRE(encoded == output);
  auto str_encoded = Base64Codec::encode(in);
  REQUIRE(str_encoded == output);

  // decoding
  BYTE_STRING decoded;
  bool success;
  std::tie(decoded, success) = Base64Codec::decode(output);

  // std::cout << "in: [" << output << "] decoded: [" << decoded << "] == [" <<
  // input << "]" << std::endl;
  REQUIRE(success == true);
  REQUIRE(decoded.size() == input.size());
  REQUIRE(decoded == input);
}

TEST_CASE("base64 encoding/decoding", "[Codec]") {

  SECTION("base64 encoding") {
    test_codec({"pleasure.", "cGxlYXN1cmUu"});
    test_codec({"leasure.", "bGVhc3VyZS4="});
    test_codec({"easure.", "ZWFzdXJlLg=="});
    test_codec({"asure.", "YXN1cmUu"});
    test_codec({"sure.", "c3VyZS4="});
  }
}