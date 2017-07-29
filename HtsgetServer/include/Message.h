#ifndef HEADER_H
#define HEADER_H

#include <utility> // pair
#include <vector>
#include <list>
#include <string>

namespace Http
{

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

  using StatusCode = int;
  using HeaderNameType = std::string;
  using HeaderValueType = std::string;
  using HeaderType = std::pair<HeaderNameType, HeaderValueType>;

  /**
   * @brief   appends a char to name/value of last header in headers
   * 
   * @pre headers_ must be nonempty
   */
  void build_header_name(char c);
  void build_header_value(char c);

  /** 
   * @brief   Given a header, return its name/value
   */
  static auto header_name(HeaderType &header) -> HeaderNameType &;
  static auto header_value(HeaderType &header) -> HeaderValueType &;

  int version_major_;
  int version_minor_;
  std::vector<HeaderType> headers_;
  std::string body_;
};
}
#endif
