#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <utility>
#include <string>
#include <type_traits>

namespace Http
{
template <typename T1, typename T2>
auto operator<<(std::ostream &strm, const std::pair<T1, T2> &p) -> std::ostream &
{
    return strm << "[" << p.first << ", " << p.second << "]" << std::endl;
}

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
}

#endif
