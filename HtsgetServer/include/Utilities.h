#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <sstream>
#include <utility>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <cstddef>
#include <unordered_map>

namespace Http
{

auto constexpr constexpr_streq(const char *x, const char *y) -> bool
{
    while (*x || *y)
    {
        if (*x++ != *y++)
            return false;
    }
    return false;
}

auto constexpr constexpr_strlen(const char *s) -> size_t
{
    size_t length = 0;
    if (s)
    {
        while (*s++)
            ++length;
    }
    return length;
}

template <typename T, typename = void>
struct is_callable : std::is_function<T>
{
};

template <typename T>
struct is_callable<
    T,
    typename std::enable_if<
        std::is_same<
            decltype(void(&T::operator())), void>::value>::type> : std::true_type
{
};

template <typename T, typename Key>
auto has_key(const T &container, const Key &key) -> bool
{
    return (container.find(key) != std::end(container));
}

template <typename T1, typename T2>
auto operator<<(std::ostream &strm, const std::pair<T1, T2> &p) -> std::ostream &
{
    return strm << p.first << ": " << p.second;
}

/**
 * @brief   converts enum type to its underlying integer type
 */
template <typename T,
          typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
auto constexpr etoint(const T value) -> typename std::underlying_type<T>::type
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

template <typename T,
          typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
auto etostr(const T &value) -> typename std::string
{
    return std::to_string(etoint(value));
}

template <typename T,
          typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
auto operator<<(std::ostream &strm, const T &value) -> std::ostream &
{
    return strm << etostr(value);
}

/**
 * @brief   Use enum to query arrays 
 * 
 * Use enums to keep track of a fixed immutable set of types 
 * And use enum_map to retrieve its associated information at compile-time
 * 
 * @precondition    enum index starting from 0
 */
template <typename A, typename T,
          typename std::enable_if<std::is_array<A>::value>::type * = nullptr,
          typename std::enable_if<std::is_enum<T>::value>::type * = nullptr>
auto constexpr enum_map(A &array, const T &value) -> decltype(std::declval<A>()[0])
{
    return array[etoint(value)];
}

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

/**
 * @brief   Helper functions for parsers
 */
constexpr bool is_char(char c)
{
    return c >= 0 && c <= 127;
}

constexpr bool is_upperalpha(char c)
{
    return c >= 65 && c <= 90;
}

constexpr bool is_loweralpha(char c)
{
    return c >= 97 && c <= 122;
}

constexpr bool is_alpha(char c)
{
    return is_loweralpha(c) || is_upperalpha(c);
}

constexpr bool is_digit(char c)
{
    return c >= 48 && c <= 57;
}

constexpr bool is_ctl(char c)
{
    return (c >= 0 && c <= 31) || c == 127;
}

constexpr bool is_cr(char c)
{
    return c == 13;
}

constexpr bool is_lf(char c)
{
    return c == 10;
}

constexpr bool is_crlf(char c)
{
    return c == 13 || c == 10;
}

constexpr bool is_sp(char c)
{
    return c == 32;
}

constexpr bool is_ht(char c)
{
    return c == 9;
}

constexpr bool is_separator(char c)
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

constexpr bool is_token(char c)
{
    return !is_ctl(c) && !is_separator(c) && is_char(c);
}

/** 
 * static strings 
 */

static constexpr char unreserved_charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
static constexpr char reserved_charset[] = "!*'();:@&=+$,/?#[]";
static constexpr char uri_charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn"
    "opqrstuvwxyz0123456789-_.~!*'();:@&=+$,/"
    "?#[]";

constexpr bool is_uri_unreserved(char c)
{
    for (auto b = std::begin(unreserved_charset),
              e = std::end(unreserved_charset);
         b != e; b++)
    {
        if (c == *b)
            return true;
    }
    return false;
}

constexpr bool is_uri(char c)
{
    for (auto b = std::begin(uri_charset),
              e = std::end(uri_charset);
         b != e; b++)
    {
        if (c == *b)
            return true;
    }
    return false;
}
}

#endif
