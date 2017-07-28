#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include "Request.h"

namespace Http
{

class RequestParser
{
  private:
    enum class State
    {
        req_start,
        req_method,
        req_uri,
        req_http_h,
        req_http_ht,
        req_http_htt,
        req_http_http,
        req_http_major,
        req_http_dot,
        req_http_minor,
        req_start_line_cr,
        req_start_line_lf,
        req_field_name,
        req_field_value,
        req_header_cr,
        req_header_lf
    };
   

  public:
  
   enum class ParseStatus
    {
        accept,
        reject,
        in_progress
    };

    explicit RequestParser()
        : state_(State::req_start){};

    /**
     * @brief Populate Request object given a Range of chars 
     */
    template <typename InputIterator>
    std::tuple<InputIterator, ParseStatus> parse(Request &request, InputIterator begin, InputIterator end)
    {
        while (begin != end)
        {
            auto status = consume(request, *begin++);
            if (status == ParseStatus::accept || status == ParseStatus::reject)
                break;

            if (begin == end)
                assert(status == ParseStatus::in_progress);
        }

        return {begin, ParseStatus::in_progress};
    }

    /** 
     * @brief   Advance parser state given input char
     */
    auto consume(Request &request, char c) -> ParseStatus;

  private:
    State state_;
    /*  Generic Message Types
        generic-message = start-line
                          *(message-header CRLF)
                          CRLF
                          [ message-body ]
        start-line      = Request-Line | Status-Line

        Message Headers
        message-header = field-name ":" [ field-value ]
        field-name     = token
        field-value    = *( field-content | LWS )
        field-content  = <the OCTETs making up the field-value
                            and consisting of either *TEXT or combinations
                            of token, separators, and quoted-string>

    */
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

    static constexpr bool is_char(char c)
    {
        return c >= 0 && c <= 127;
    }

    static constexpr bool is_upperalpha(char c)
    {
        return c >= 65 && c <= 90;
    }

    static constexpr bool is_loweralpha(char c)
    {
        return c >= 97 && c <= 122;
    }

    static constexpr bool is_alpha(char c)
    {
        return is_loweralpha(c) || is_upperalpha(c);
    }

    static constexpr bool is_digit(char c)
    {
        return c >= 48 && c <= 57;
    }

    static constexpr bool is_ctl(char c)
    {
        return (c >= 0 && c <= 31) || c == 127;
    }

    static constexpr bool is_cr(char c)
    {
        return c == 13;
    }

    static constexpr bool is_lf(char c)
    {
        return c == 10;
    }

    static constexpr bool is_sp(char c)
    {
        return c == 32;
    }

    static constexpr bool is_ht(char c)
    {
        return c == 9;
    }

    static constexpr bool is_separator(char c)
    {
        switch (c)
        {
        case '(':
        case ')':
        case '<':
        case '>':
        case '@':
        case ',':
        case ';':
        case ':':
        case '\\':
        case '"':
        case '/':
        case '[':
        case ']':
        case '?':
        case '=':
        case '{':
        case '}':
        case ' ':
        case '\t':
            return true;
        default:
            return false;
        }
    }

    static constexpr bool is_token(char c)
    {
        return !is_ctl(c) && !is_separator(c) && is_char(c);
    }
};
}

#endif