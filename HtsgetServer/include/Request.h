#ifndef REQUEST_H
#define REQUEST_H

#include <unordered_map>
#include <string>

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
  std::unordered_map<std::string, std::string> param_;

  constexpr static const char *request_method_to_string(RequestMethod method)
  {
    return enum_map(request_methods, method);
  };

  friend std::ostream &operator<<(std::ostream &strm, const Request &request);
};
}

#endif