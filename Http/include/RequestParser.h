#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <algorithm>
#include <iostream>
#include <cassert>

#include "Request.h"
#include "Constants.h"

namespace Http
{

class RequestParser
{
  public:
    enum class State;

    explicit RequestParser() : state_(State::req_start){};

    State state_;

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

    static auto view_state(RequestParser::State state, ParseStatus status, char c) -> void;

    friend auto operator<<(std::ostream &strm, ParseStatus &status) -> std::ostream &;

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

  private:
};
}

#endif