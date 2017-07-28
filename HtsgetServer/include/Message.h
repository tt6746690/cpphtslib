#ifndef HEADER_H
#define HEADER_H

#include <utility> // pair

namespace Http
{

using Header = std::pair<std::string, std::string>;

enum class GetHeader
{
    NAME,
    VALUE
};

using Headers = std::vector<Header>;
using StatusCode = int;

/*  HTTP-message   = Request | Response */
class Message
{
};
}
#endif
