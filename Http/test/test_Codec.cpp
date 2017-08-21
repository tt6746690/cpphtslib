#include "catch.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "Codec.h"

using namespace Http;

auto test_base64(std::pair<std::string, std::string> in_out) -> void {

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

  // std::cout << "in: [" << output << "] decoded: [" << decoded << "] == ["
  //           << input << "]" << std::endl;
  REQUIRE(success == true);
  REQUIRE(decoded.size() == input.size());
  REQUIRE(decoded == input);
}

TEST_CASE("base64 encoding/decoding", "[Codec]") {

  SECTION("base64 encoding") {
    test_base64({"pleasure.", "cGxlYXN1cmUu"});
    test_base64({"leasure.", "bGVhc3VyZS4="});
    test_base64({"easure.", "ZWFzdXJlLg=="});
    test_base64({"asure.", "YXN1cmUu"});
    test_base64({"sure.", "c3VyZS4="});
  }
}

auto test_sha256(std::pair<std::string, > in_out) -> void {

  auto in = std::get<0>(in_out);
  auto out = std::get<1>(in_out);

  auto message = BYTE_STRING(in.begin(), in.end());
  auto output = BYTE_STRING(out.begin(), out.end());

  SHA256Codec sha;
  auto digest = sha.digest(message);

  std::cout << "message: " << message << std::endl;
  std::cout << "correct out: " << output << std::endl;
  std::cout << "digest: " << digest << std::endl;

  REQUIRE(digest == output);
}

TEST_CASE("sha256 digest", "[Codec]") {
  SECTION("sha256 digest") {
    test_sha256({"", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7"
                     "852b855"});
    test_sha256({"helloworld", "8cd07f3a5ff98f2a78cfc366c13fb123eb8d29c1ca3"
                               "7c79df190425d5b9e424d"});
  }
}