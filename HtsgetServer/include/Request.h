#ifndef REQUEST_H
#define REQUEST_H

#include "Message.h"

namespace Http
{

class Request : public Message
{
public:
  std::string method;
  std::string uri;
  std::string version;
  Headers headers;
  std::string body;
};
}

#endif