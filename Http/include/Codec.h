#ifndef CODEC_H
#define CODEC_H

// reference http://web.mit.edu/freebsd/head/contrib/wpa/src/utils/base64.c

#include <ostream>
#include <string>

#include <cassert>
#include <cstdint>
#include <cstring>

namespace Http {

using BYTE = uint8_t;
using BYTE_STRING = std::basic_string<BYTE>;

auto inline operator<<(std::ostream &strm, const BYTE_STRING in)
    -> std::ostream & {
  static int blkof4 = 0;
  for (const auto c : in) {
    strm << c - 0;
    if (blkof4++ == 4) {
      strm << " ";
      blkof4 = 0;
    }
  }
  blkof4 = 0;
  return strm;
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
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57,
      // `=`
      58, 59, 60, 61, 64, 64, 64, 0, 64, 64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64,
      64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64};

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

// reference : http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf

using WORD = uint32_t;
using WORD_STRING = std::basic_string<WORD>;

class SHA256Codec {

private:
  static constexpr int BLOCK_SIZE = (512 / 8); // 64 byte blocks
  static constexpr WORD ROTL(WORD x, int n) {
    return (x << n) | (x >> (32 - n));
  };
  static constexpr WORD ROTR(WORD x, int n) {
    return (x >> n) | (x << (32 - n));
  };
  static constexpr WORD SHR(WORD x, int n) { return (x >> n); }
  static constexpr WORD CH(WORD x, WORD y, WORD z) {
    return (x & y) ^ (~x & z);
  }
  static constexpr WORD MAJ(WORD x, WORD y, WORD z) {
    return (x & y) ^ (x & z) ^ (y & z);
  }
  // Hash functions
  static constexpr WORD SIGMA0(WORD x) {
    return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22);
  }
  static constexpr WORD SIGMA1(WORD x) {
    return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25);
  }
  static constexpr WORD EPSILON0(WORD x) {
    return ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3);
  }
  static constexpr WORD EPSILON1(WORD x) {
    return ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10);
  }
  // first 32 bits of fractional parts of cube roots of first 64 prime numbers
  static constexpr WORD K[64] = {
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
      0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
      0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
      0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
      0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
      0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
      0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
      0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
      0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

private:
  constexpr static int DIGEST_SIZE = 32; // 32 bytes = 8 32bit words

  /**
   * @brief   Use a message schedule of 64 32bit words to
   *          transform hash_ to next state
   */
  auto inline transform() -> void {
    WORD a, b, c, d, e, f, g, h, t1, t2, W[64];

    for (int i = 0, j = 0; i < 16; ++i, j += 4) {
      W[i] = (blk_[j] << 24) | (blk_[j + 1]) << 16 | (blk_[j + 2]) << 8 |
             (blk_[j + 3]);
    }
    for (int i = 16; i < 64; ++i) {
      W[i] = SIGMA1(W[i - 2]) + W[i - 7] + SIGMA0(W[i - 15]) + W[i - 16];
    }

    a = hash_[0];
    b = hash_[1];
    c = hash_[2];
    d = hash_[3];
    e = hash_[4];
    f = hash_[5];
    g = hash_[6];
    h = hash_[7];

    for (int i = 0; i < 64; ++i) {
      t1 = h + SIGMA1(e) + CH(e, f, g) + K[i] + W[i];
      t2 = SIGMA0(a) + MAJ(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
    }

    hash_[0] += a;
    hash_[1] += b;
    hash_[2] += c;
    hash_[3] += d;
    hash_[4] += e;
    hash_[5] += f;
    hash_[6] += g;
    hash_[7] += h;
  }

  /**
   * @brief   Update hash_ by iterating over message blocks of M
   */
  auto inline update(const BYTE_STRING &M) -> void {

    auto l = M.size();

    for (int i = 0; i < l; ++i) {
      blk_ += M[i];
      if (blk_.size() == 64) {
        transform();
        blk_.clear();
        assert(blk_.size() == 0);
        bit_length_ += 512;
      }
    }
    bit_length_ += blk_.size() * 8;

    std::cout << "after update, blk_=" << blk_ << " bitlength=" << bit_length_
              << std::endl;
  }

  /**
   * @brief   Pad message M,
   *          parse into message blks, and
   *          setting initial hash value H
   */
  auto inline pad() -> void {

    if (blk_.size() < 56) {
      blk_ += static_cast<uint8_t>(0x80);
      while (blk_.size() < 56)
        blk_ += static_cast<uint8_t>(0x00);
    } else {
      blk_ += static_cast<uint8_t>(0x80);
      while (blk_.size() < 64)
        blk_ += static_cast<uint8_t>(0x00);
      transform();
      blk_.clear();
      while (blk_.size() < 56)
        blk_ += static_cast<uint8_t>(0x00);
    }
    assert(blk_.size() == 56);

    std::cout << "[" << blk_ << "]" << std::endl;

    // last 8 bytes of blk_ holds message length in bits
    blk_ += bit_length_ & 0xff;
    blk_ += (bit_length_ >> 8) & 0xff;
    blk_ += (bit_length_ >> 16) & 0xff;
    blk_ += (bit_length_ >> 24) & 0xff;
    blk_ += (bit_length_ >> 32) & 0xff;
    blk_ += (bit_length_ >> 40) & 0xff;
    blk_ += (bit_length_ >> 48) & 0xff;
    blk_ += (bit_length_ >> 56) & 0xff;

    std::cout << "[" << blk_ << "]" << std::endl;

    assert(blk_.size() == 64);
    transform();
  }

  /**
   * @brief   Prints current hash value
   */
  inline void print_hash(std::string msg) {
    std::cout << msg << ": ";
    for (int i = 0; i < 7; ++i) {
      std::cout << std::hex << hash_[i] << " ";
    }
    std::cout << std::endl;
  }

public:
  /**
   * @brief   Generate digests
   */
  auto inline digest(const BYTE_STRING &M) -> BYTE_STRING {

    print_hash("init: ");

    update(M);
    print_hash("after update");

    pad();
    print_hash("after pad");

    std::vector<BYTE> hash(32);

    for (int i = 0; i < 7; ++i) {
      hash[i] = (hash_[0] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 4] = (hash_[1] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 8] = (hash_[2] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 12] = (hash_[3] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 16] = (hash_[4] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 20] = (hash_[5] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 24] = (hash_[6] >> (24 - i * 8)) & 0x000000ff;
      hash[i + 28] = (hash_[7] >> (24 - i * 8)) & 0x000000ff;
    }

    return BYTE_STRING(hash.begin(), hash.end());
  }

private:
  BYTE_STRING blk_;
  WORD hash_[8] = {
      0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
      0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
  };
  int64_t bit_length_ = 0;
};
}

#endif
