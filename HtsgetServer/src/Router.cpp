#include <type_traits>
#include <iostream>
#include <utility>

#include "Router.h"

#include "Request.h"
#include "Response.h"
#include "Constants.h"

namespace Http
{

// auto handler = [](Request &, Response &) {};
// auto handlerfunc(Request &, Response &) -> void{};

// using HandlerFunc = decltype(handlerfunc);
// using HandlerLambda = decltype(handler);

// if (std::is_same<handlerFunc, HandlerLambda>::value)
// {
//     std::cout << "same!" << std::endl;
// }

// std::cout << "works!" << std::endl;
}