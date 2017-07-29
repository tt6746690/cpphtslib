#ifndef RESPONSE_H
#define RESPONSE_H

#include <array>
#include "Message.h"

namespace Http
{

class Response : public Message
{

  enum class ResponseHeaderName
  {
    Access_Control_Allow_Origin,
    Access_Control_Allow_Credentials,
    Access_Control_Expose_Headers,
    Access_Control_Max_Age,
    Access_Control_Allow_Methods,
    Access_Control_Allow_Headers,
    Accept_Patch,
    Accept_Ranges,
    Age,
    Allow,
    Alt_Svc,
    Cache_Control,
    Connection,
    Content - Disposition,
    Content_Encoding,
    Content_Language,
    Content_Length,
    Content_Location,
    Content_Range,
    Content_Type,
    Date,
    ETag,
    Expires,
    Last_Modified,
    Link,
    Location,
    P3P,
    Pragma,
    Proxy_Authenticate,
    Public_Key_Pins,
    Retry_After,
    Server,
    Set_Cookie,
    Strict_Transport_Security,
    Trailer,
    Transfer_Encoding,
    Tk,
    Upgrade,
    Vary,
    Via,
    Warning,
    WWW_Authenticate
  }

  public : StatusCode status_code_;
  std::string reason_;
  std::array<std::string, 3> payload_;

  friend std::ostream &operator<<(std::ostream &strm, const Response &response);
};
}

#endif