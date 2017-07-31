#ifndef REQUEST_H
#define REQUEST_H

#include "Message.h"
#include "Utilities.h"
#include "Constants.h"

namespace Http
{

class Request : public Message
{

public:
  constexpr static const char *request_method(RequestMethod method)
  {
    return enum_map(request_methods, method);
  }

  std::string method_;
  std::string uri_;

  friend std::ostream &operator<<(std::ostream &strm, const Request &request);
};
}

#endif