#ifndef RESPONSE_H
#define RESPONSE_H

#include <array>
#include "Message.h"

namespace Http
{

class Response : public Message
{
public:
  using BuildStatus = int;
  enum class StatusCode;
  enum class ResponseHeaderName;

  // using StatusCode = int;
  /**
   * @brief   Gets status line 
   * 
   * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
   */
  std::string
  status_line() const;
  std::string static status_line(
      StatusCode status_code,
      std::string reason,
      std::string http_version_major = "1",
      std::string http_version_minor = "1");

  StatusCode status_code_;
  std::string reason_;
  std::array<std::string, 3> payload_;

  enum class StatusCode
  {
    Continue = 100,
    Switching_Protocols = 101,
    OK = 200,
    Created = 201,
    Accepted = 202,
    Non_Authoritative_Information = 203,
    No_Content = 204,
    Reset_Content = 205,
    Partial_Content = 206,
    Multiple_Choices = 300,
    Moved_Permanently = 301,
    Found = 302,
    See_Other = 303,
    Not_Modified = 304,
    Use_Proxy = 305,
    Temporary_Redirect = 307,
    Bad_Request = 400,
    Unauthorized = 401,
    Payment_Required = 402,
    Forbidden = 403,
    Not_Found = 404,
    Method_Not_Allowed = 405,
    Not_Acceptable = 406,
    Proxy_Authentication_Required = 407,
    Request_Timeout = 408,
    Conflict = 409,
    Gone = 410,
    Length_Required = 411,
    Precondition_Failed = 412,
    Request_Entity_Too_Large = 413,
    Request_URI_Too_Large = 414,
    Unsupported_Media_Type = 415,
    Requested_Range_Not_Satisfiable = 416,
    Expectation_Failed = 417,
    Internal_Server_Error = 500,
    Not_Implemented = 501,
    Bad_Gateway = 502,
    Service_Unavailable = 503,
    Gateway_Timeout = 504,
    HTTP_Version_Not_Supported = 505
  };

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
    Content_Disposition,
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
  };

  friend std::ostream &operator<<(std::ostream &strm, const Response &response);
};
}

#endif