#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <algorithm>
#include <iostream>
#include <cassert>

#include "Request.h"

namespace Http
{

static constexpr char unreserved_charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
static constexpr char reserved_charset[] = "!*'();:@&=+$,/?#[]";
static constexpr char uri_charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn"
    "opqrstuvwxyz0123456789-_.~!*'();:@&=+$,/"
    "?#[]";

class RequestParser
{
  public:
    enum class ParseStatus
    {
        accept = 1,
        reject,
        in_progress
    };

    enum class State
    {
        req_start = 1,        // 1
        req_start_lf,         // 2
        req_method,           // 3
        req_uri,              // 4
        req_http_h,           // 5
        req_http_ht,          // 6
        req_http_htt,         // 7
        req_http_http,        // 8
        req_http_slash,       // 9
        req_http_major,       // 10
        req_http_dot,         // 11
        req_http_minor,       // 12
        req_start_line_cr,    // 13
        req_start_line_lf,    // 14
        req_field_name_start, // 15
        req_field_name,       // 16
        req_field_value,      // 17
        req_header_lf,        // 18
        req_header_lws,       // 19
        req_header_end        // 20
    };

    explicit RequestParser() : state_(State::req_start){};

    /**
   * @brief Populate Request object given a Range of chars
   */
    template <typename InputIterator>
    auto parse(Request &request, InputIterator begin, InputIterator end)
        -> std::tuple<InputIterator, ParseStatus>
    {
        ParseStatus status;
        while (begin != end)
        {
            status = consume(request, *begin++);
            if (status == ParseStatus::accept || status == ParseStatus::reject)
                break;
        }
        return {begin, status};
    }

    /**
     * @brief   Advance parser state given input char
     */
    auto consume(Request &request, char c) -> ParseStatus;

    static auto view_state(RequestParser::State state, RequestParser::ParseStatus status, char c) -> void;
    State state_;

  private:
    /*
      OCTET          = <any 8-bit sequence of data>
      CHAR           = <any US-ASCII character (octets 0 - 127)>
      UPALPHA        = <any US-ASCII uppercase letter "A".."Z">
      LOALPHA        = <any US-ASCII lowercase letter "a".."z">
      ALPHA          = UPALPHA | LOALPHA
      DIGIT          = <any US-ASCII digit "0".."9">
      CTL            = <any US-ASCII control character
                          (octets 0 - 31) and DEL (127)>
      CR             = <US-ASCII CR, carriage return (13)>
      LF             = <US-ASCII LF, linefeed (10)>
      SP             = <US-ASCII SP, space (32)>
      HT             = <US-ASCII HT, horizontal-tab (9)>
      <">            = <US-ASCII double-quote mark (34)>

      CRLF           = CR LF
      LWS            = [CRLF] 1*( SP | HT )
      TEXT           = <any OCTET except CTLs,
                      but including LWS>

      token          = 1*<any CHAR except CTLs or separators>
      separators     = "(" | ")" | "<" | ">" | "@"
                     | "," | ";" | ":" | "\" | <">
                     | "/" | "[" | "]" | "?" | "="
                     | "{" | "}" | SP | HT
  */

    static constexpr bool is_char(char c);
    static constexpr bool is_upperalpha(char c);
    static constexpr bool is_loweralpha(char c);
    static constexpr bool is_alpha(char c);
    static constexpr bool is_digit(char c);
    static constexpr bool is_ctl(char c);
    static constexpr bool is_cr(char c);
    static constexpr bool is_lf(char c);
    static constexpr bool is_crlf(char c);
    static constexpr bool is_sp(char c);
    static constexpr bool is_ht(char c);
    static constexpr bool is_separator(char c);
    static constexpr bool is_token(char c);
    static constexpr bool is_uri(char c);
};

constexpr inline std::ostream &operator<<(std::ostream &strm,
                                          RequestParser::ParseStatus &status)
{
    switch (status)
    {
    case RequestParser::ParseStatus::accept:
        strm << "[Accept = ";
        break;
    case RequestParser::ParseStatus::reject:
        strm << "[Reject = ";
        break;
    case RequestParser::ParseStatus::in_progress:
        strm << "[In progress = ";
        break;
    default:
        break;
    }
    return strm
           << static_cast<std::underlying_type<RequestParser::ParseStatus>::type>(
                  status)
           << "]";
}
}

#endif