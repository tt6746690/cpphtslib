#ifndef HEADER_H
#define HEADER_H

#include <utility> // pair
#include <vector>
#include <list>
#include <string>

namespace Http
{

class Message
{
public:
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

  /**
   * @brief   Return HTTP version 
   */
  static auto version(int major, int minor) -> std::string;

  int version_major_;
  int version_minor_;
  std::list<HeaderType> headers_;
  std::string body_;
};
}
#endif
