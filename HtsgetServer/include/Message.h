#ifndef HEADER_H
#define HEADER_H

#include <utility> // pair
#include <vector>
#include <string>

namespace Http
{

using StatusCode = int;

/*  HTTP-message   = Request | Response */
class Message
{
  public:
    // class Headers {
    //     public:
    //         using HeaderNameType = std::string;
    //         using HeaderValueType = std::string;
    //         using HeaderType = std::pair<HeaderNameType, HeaderValueType>;

    //         inline std::optional<HeaderValueType> operator[](const HeaderNameType name) const {
    //             auto found = find_first_if(headers_.begin(), headers_.end(),
    //                 [](HeaderType& header){
    //                     return std::get<0>header == name
    //                 })
    //             if(found != headers_end())
    //                 return std::get<1>found;
    //             return {};
    //         }

    //     private:
    //         std::vector<HeaderType> headers_;
    // }

    using HeaderNameType = std::string;
    using HeaderValueType = std::string;
    using HeaderType = std::pair<HeaderNameType, HeaderValueType>;

    std::vector<HeaderType> headers;
};
}
#endif
