#ifndef CODEC_H
#define CODEC_H

// reference http://web.mit.edu/freebsd/head/contrib/wpa/src/utils/base64.c

#include <ostream>
#include <string>

namespace Http {

using BYTE = unsigned char;
using BYTE_STRING = std::basic_string<BYTE>;

auto inline operator<<(std::ostream &strm, BYTE_STRING in) -> std::ostream & {
  return strm << std::string(in.begin(), in.end());
}

class Base64Codec {

private:
  /**
   * @brief   Converts from a 3 1b (BYTE) to 4 1b base64
   */
  static constexpr auto enc1(const BYTE *pos) -> BYTE { return (pos[0] >> 2); }
  static constexpr auto enc2(const BYTE *pos) -> BYTE {
    return ((pos[0] & 0x03) << 4) | (pos[1] >> 4);
  }
  static constexpr auto enc3(const BYTE *pos) -> BYTE {
    return ((pos[1] & 0x0f) << 2) | (pos[2] >> 6);
  }
  static constexpr auto enc4(const BYTE *pos) -> BYTE {
    return (pos[2] & 0x3f);
  }

  /**
   * @brief   Converts from 4 1b base 64 (i.e. 0~63) to a 3 1b (BYTE)
   */
  static constexpr auto dec1(const BYTE *pos) -> BYTE {
    return ((pos[0] << 2) | (pos[1] >> 4));
  }
  static constexpr auto dec2(const BYTE *pos) -> BYTE {
    return ((pos[1] << 4) | (pos[2] >> 2));
  }
  static constexpr auto dec3(const BYTE *pos) -> BYTE {
    return ((pos[2] << 6) | pos[3]);
  }

  static constexpr char base64_pad = '=';
  static constexpr BYTE encode_table[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static constexpr BYTE decode_table[256] = {
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      // '+'              '/'
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
      //  '0'                                 '9'             '='
      52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 0, 64, 64,
      //  'A'
      64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
      20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
      //  'a'
      64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
      43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};

public:
  /**
   * @brief   Encode data to base64 encoding
   *
   *          3 bytes (24 bits) -> 4 base64 character (6 bits)
   */
  auto static inline encode(const BYTE_STRING &input) -> BYTE_STRING {
    BYTE_STRING encoded;

    auto len = input.size();
    encoded.reserve(4 * ((len / 3) + (len % 3 > 0)));

    auto ptr = input.data();
    auto end = ptr + input.size();

    while (end - ptr >= 3) {
      encoded += encode_table[enc1(ptr)];
      encoded += encode_table[enc2(ptr)];
      encoded += encode_table[enc3(ptr)];
      encoded += encode_table[enc4(ptr)];
      ptr += 3;
    }

    switch (len % 3) {
    case 1: {
      encoded += encode_table[enc1(ptr)];
      encoded += encode_table[enc2(ptr)];
      encoded += base64_pad;
      encoded += base64_pad;
      break;
    }
    case 2: {
      encoded += encode_table[enc1(ptr)];
      encoded += encode_table[enc2(ptr)];
      encoded += encode_table[enc3(ptr)];
      encoded += base64_pad;
      break;
    }
    }

    return encoded;
  }

  auto static inline encode(const std::string &in) -> BYTE_STRING {
    const auto input = BYTE_STRING(in.begin(), in.end());
    return encode(input);
  }

  /**
   * @brief   Decode data from base64 encoding
   *
   *          4 base64 character (6 bits) -> 3 bytes (24 bits)
   */
  auto static inline decode(const BYTE_STRING input)
      -> std::pair<BYTE_STRING, bool> {

    BYTE_STRING decoded;

    if (input.size() % 4)
      return std::make_pair(decoded, false);

    BYTE_STRING chunk4b;
    chunk4b.reserve(4);
    auto start = chunk4b.data();

    auto ptr = input.begin();
    while (ptr < input.end()) {

      chunk4b[0] = decode_table[*ptr];
      chunk4b[1] = decode_table[*(ptr + 1)];
      chunk4b[2] = decode_table[*(ptr + 2)];
      chunk4b[3] = decode_table[*(ptr + 3)];

      decoded += dec1(start);
      decoded += dec2(start);
      decoded += dec3(start);

      ptr += 4;
    }

    // remove trailing whitespace
    ptr = decoded.end();
    while (*(ptr - 1) == 0) {
      --ptr;
    }
    decoded.erase(ptr, input.end());

    return std::make_pair(decoded, true);
  }
};
}

#endif
