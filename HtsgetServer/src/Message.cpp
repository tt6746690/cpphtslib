#include <utility>
#include <iostream>
#include <cassert>

#include "Message.h"

namespace Http
{

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
}