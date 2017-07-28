#ifndef RESPONSE_H
#define RESPONSE_H

#include "Message.h"

namespace Http
{

class Response : public Message
{

public:
  StatusCode status_code_;
  std::string reason_;

  friend std::ostream &operator<<(std::ostream &strm, const Response &response);
};
}

#endif