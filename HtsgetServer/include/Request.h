#ifndef REQUEST_H
#define REQUEST_H

#include "Message.h"

namespace Http
{

class Request : public Message
{

public:
  enum class RequestHeaderName;

  std::string method_;
  std::string uri_;

  enum class RequestHeaderName
  {
    Accept,
    Accept_Charset,
    Accept_Encoding,
    Accept_Language,
    Access_Control_Request_Method,
    Access_Control_Request_Headers,
    Authorization,
    Cache_Control,
    Connection,
    Cookie,
    Content_Length,
    Content_Encoding,
    Content_Type,
    Date,
    Expect,
    Forwarded,
    From,
    Host,
    If_Match,
    If_Modified_Since,
    If_None_Match,
    If_Range,
    If_Unmodified_Since,
    Max_Forwards,
    Origin,
    Pragma,
    Proxy_Authorization,
    Range,
    Referer,
    TE,
    User_Agent,
    Upgrade,
    Via,
    Warning
  };

  friend std::ostream &operator<<(std::ostream &strm, const Request &request);
};
}

#endif