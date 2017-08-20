#ifndef REQUEST_H
#define REQUEST_H

#include <ostream>
#include <string>
#include <unordered_map>

#include "Constants.h" // RequestMetho
#include "Message.h"   // base class
#include "Uri.h"       // Uri
#include "Utilities.h" // enum_map

namespace Http {

class Request : public Message {

public:
  RequestMethod method_ = RequestMethod::UNDETERMINED;

  Uri uri_;
  ssmap param_;
  ssmap query_;

public:
  constexpr static const char *request_method_to_string(RequestMethod method) {
    return enum_map(request_methods, method);
  };

  friend auto inline operator<<(std::ostream &strm, const Request &request)
      -> std::ostream & {
    strm << "> " << Request::request_method_to_string(request.method_) << " "
         << request.uri_ << " "
         << Message::version(request.version_major_, request.version_minor_)
         << std::endl;
    for (auto header : request.headers_) {
      strm << "> " << header << std::endl;
    }
    strm << "> " << request.body_ << std::endl;

    return strm;
  }
};
}

#endif