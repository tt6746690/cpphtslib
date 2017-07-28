#ifndef REQUEST_H
#define REQUEST_H

#include "Message.h"

namespace Http
{

class Request : public Message
{

public:
  std::string method_;
  std::string uri_;

  friend std::ostream &operator<<(std::ostream &strm, const Request &request);
};
}

#endif