#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <utility>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace Http
{
template <typename T1, typename T2>
auto operator<<(std::ostream &strm, const std::pair<T1, T2> &p) -> std::ostream &
{
    return strm << "[" << p.first << ", " << p.second << "]" << std::endl;
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
}

#endif
