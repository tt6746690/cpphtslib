#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <utility>

namespace Http
{
template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &strm, const std::pair<T1, T2> &p)
{
    return strm << "[" << p.first << ", " << p.second << "]" << std::endl;
}

template <typename Enumeration>
auto etoint(const Enumeration value) -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}
}

#endif
