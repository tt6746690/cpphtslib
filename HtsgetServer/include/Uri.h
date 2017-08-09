#ifndef URI_H
#define URI_H

#include <string>
#include <ostream>

#include "Constants.h"

namespace Http
{

/**
 * @brief   Converts 1 utf8 byte to its hex value
 */
auto ctohex(unsigned int c) -> std::string;

class Uri
{
public:
  /* Assume only accepting Url for now*/
  std::string scheme_;
  std::string host_;
  std::string port_;
  std::string abs_path_;
  std::string query_;
  std::string fragment_;

  UriState state_ = UriState::uri_start;

  /**
   * @brief   Advance state for parsing uri given char 
   * 
   * @precondition  c is valid uri char
   */
  auto consume(char c) -> ParseStatus;

  /**
   * @brief   Decodes fields in uri 
   */
  auto decode() -> void;
  /**
   * @brief   encode url
   * 
   * @precond assumes utf8 encoded string 
   *          assumes url consists of uri allowed charset
   * 
   * -- No need to encode unreserved charset
   * -- Percent encode reserved charset, 
   *  --  convert each char (ASCII or non-ASCII) to utf-8
   *  --  Represenet byte value with hex digits, preceded by %
   */
  auto static urlencode(std::string &url) -> std::string;
  /**
   * @brief   decode url
   * 
   * @precond assumes url consists of uri allowed charset
   */
  auto static urldecode(std::string &url) -> std::string;

  friend auto operator<<(std::ostream &strm, Uri uri) -> std::ostream &;
};
}

#endif