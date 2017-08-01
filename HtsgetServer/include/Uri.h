#ifndef URI_H
#define URI_H

#include <string>

#include "Constants.h"

namespace Http
{

class Uri
{
public:
  /* Assume only accepting Url for now*/
  std::string url_;
  std::string scheme_;
  std::string host_; // host or host:port
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
};
}

#endif