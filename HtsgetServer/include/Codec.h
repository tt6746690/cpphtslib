#ifndef ENCODING_H
#define ENCODING_H

#include <string>
#include <ostream>

namespace Http {

using byte_array = std::basic_string<unsigned char>;

auto inline operator<<(std::ostream& strm, byte_array in) -> std::ostream& {
    return strm << std::string(in.begin(), in.end());
}


class Base64Codec {

private:
    static constexpr unsigned char base64_table[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static constexpr char base64_pad = '=';

    static constexpr auto first6bit(const unsigned char* ptr) -> int {
        return (ptr[0] >> 2);
    }
    static constexpr auto second6bit(const unsigned char* ptr) -> int {
        return ((ptr[0] & 0x03) << 4) | (ptr[1] >> 4);
    }
    static constexpr auto third6bit(const unsigned char* ptr) -> int {
        return ((ptr[1] & 0x0f) << 2) | (ptr[2] >> 6);
    }
    static constexpr auto fourth6bit(const unsigned char* ptr) -> int {
        return (ptr[2] & 0x3f);
    }

public:
    /**
     * @brief   Encode data to base64 encoding
     */
    auto static inline encode(const byte_array& input) -> byte_array {
        byte_array encoded;

        auto len = input.size();
        encoded.reserve(4 * ((len / 3) + (len % 3 > 0)));

        auto ptr = input.data();
        auto end = ptr + input.size();
        
        while(end - ptr >= 3){
            encoded += base64_table[first6bit(ptr)];
            encoded += base64_table[second6bit(ptr)];
            encoded += base64_table[third6bit(ptr)];
            encoded += base64_table[fourth6bit(ptr)];
            ptr += 3;
        }

        switch(len % 3){
            case 1: {
                encoded += base64_table[first6bit(ptr)];
                encoded += base64_table[second6bit(ptr)];
                encoded += base64_pad;
                encoded += base64_pad;
                break;
            }
            case 2: {
                encoded += base64_table[first6bit(ptr)];
                encoded += base64_table[second6bit(ptr)];
                encoded += base64_table[third6bit(ptr)];
                encoded += base64_pad;
                break;
            }
        }

        return encoded;
    }

    auto static inline encode(const std::string& in) -> byte_array {
        const auto input = byte_array(in.begin(), in.end());
        return encode(input);
    }

};







}




#endif 
