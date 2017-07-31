#ifndef RESPONSE_H
#define RESPONSE_H

#include <array>
#include "asio.hpp"

#include "Message.h"
#include "Utilities.h"
#include "Constants.h"

namespace Http
{

class Response : public Message
{
public:
  using BuildStatus = int;

  /**
   * @brief   Generates response string
   */
  auto
  to_payload() const -> std::string;
  /**
   * @brief   Sets status code for response
   */
  void set_status_code(StatusCode status_code);

private:
  StatusCode status_code_;

public:
  /**
   * @brief   Gets status line 
   * 
   * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
   */
  auto status_line() const -> std::string;
  auto static status_line(
      StatusCode status_code,
      int http_version_major = 1,
      int http_version_minor = 1) -> std::string;

  /**
   * @brief   Gets numeric status code given StatusCode
   */
  auto static constexpr status_code(StatusCode status_code) -> int
  {
    return enum_map(status_codes, status_code);
  }
  /**
   * @brief   Gets reason phrase given StatusCode
   */
  auto static constexpr reason_phrase(StatusCode status_code) -> char *
  {
    return enum_map(reason_phrases, status_code);
  }

  friend std::ostream &operator<<(std::ostream &strm, const Response &response);
};
}

#endif