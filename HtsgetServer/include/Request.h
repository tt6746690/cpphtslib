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
  int version_major;
  int version_minor;
  std::string body;
};
}

#endif