#ifndef REQUEST_H
#define REQUEST_H

#include "Message.h"   // base class
#include "Utilities.h" // enum_map
#include "Constants.h" // RequestMetho
#include "Uri.h"       // Uri

namespace Http
{

class Request : public Message
{

public:
  RequestMethod method_ = RequestMethod::UNDETERMINED;
  Uri uri_;

  constexpr static const char *request_method(RequestMethod method)
  {
    return enum_map(request_methods, method);
  };

  friend std::ostream &operator<<(std::ostream &strm, const Request &request);
};
}

#endif