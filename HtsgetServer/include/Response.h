#ifndef RESPONSE_H
#define RESPONSE_H

#include "Message.h"

namespace Http
{

class Response : public Message
{

public:
  std::string version;
  StatusCode status_code;
  std::string reason;

  Headers headers;
  std::string body;
};
}

#endif