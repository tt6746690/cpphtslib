#include <iostream>
#include <cassert>
#include <string>
#include <utility>
#include <algorithm>

#include "Message.h"

namespace Http
{

auto Message::version(int major, int minor) -> std::string
{
    return "HTTP/" + std::to_string(major) + "." + std::to_string(minor);
}

void Message::build_header_name(char c)
{
    assert(headers_.size() != 0);
    header_name(headers_.back()).push_back(c);
}
void Message::build_header_value(char c)
{
    assert(headers_.size() != 0);
    header_value(headers_.back()).push_back(c);
}

auto Message::header_name(HeaderType &header) -> Message::HeaderNameType &
{
    return std::get<0>(header);
}
auto Message::header_value(HeaderType &header) -> Message::HeaderValueType &
{
    return std::get<1>(header);
}

auto Message::get_header(HeaderNameType name) -> std::pair<HeaderValueType, bool>
{
    HeaderValueType val{};
    bool valid = false;

    for (auto &header : headers_)
    {
        if (header_name(header) == name)
            val = header_value(header), valid = true;
    }
    return std::make_pair(val, valid);
}

void Message::set_header(HeaderNameType name, HeaderValueType value)
{
    auto header = std::make_pair(name, value);
    auto found = find_if(headers_.begin(), headers_.end(),
                         [&](auto &header) {
                             return header.first == name;
                         });
    if (found != headers_.end())
        *found = header;
    else
        headers_.push_back(header);
}

void Message::unset_header(HeaderNameType name)
{
    auto end = std::remove_if(headers_.begin(), headers_.end(),
                              [&](auto &header) {
                                  return header.first == name;
                              });
    headers_.erase(end, headers_.end());
}

auto Message::content_length() -> int
{
    HeaderValueType val = "";
    bool found;
    std::tie(val, found) = get_header("Content-Length");

    if (found)
        return std::atoi(val.c_str());
    else
        set_header("Content-Length", "0");
    return 0;
}

void Message::content_length(int length)
{
    set_header("Content-Length", std::to_string(length));
}

auto Message::content_type() -> HeaderValueType
{
    HeaderValueType val = "";
    bool found;
    std::tie(val, found) = get_header("Content-Type");

    if (found)
        return val;
    else
        set_header("Content-Type", "");
    return "";
}

void Message::content_type(HeaderValueType value)
{
    set_header("Content-Type", value);
}

auto operator<<(std::ostream &strm, Message::HeaderType &header) -> std::ostream &
{
    return strm << Message::header_name(header) << ": " << Message::header_value(header) << std::endl;
}
}