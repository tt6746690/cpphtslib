#ifndef RESPONSE_H
#define RESPONSE_H

#include <array>

#include "Message.h"
#include "Utilities.h"

namespace Http
{

class Response : public Message
{
public:
  using BuildStatus = int;
  enum class StatusCode;
  enum class ResponseHeaderName;

  /**
   * @brief   Gets status line 
   * 
   * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
   */
  auto status_line() const -> std::string;
  auto static status_line(
      StatusCode status_code,
      std::string reason,
      std::string http_version_major = "1",
      std::string http_version_minor = "1") -> std::string;

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

  auto static constexpr status_code(StatusCode status_code) -> int
  {
    return etoint(status_code);
  }

  /**
   * @brief   Gets reason phrase given StatusCode
   */
  auto static constexpr reason_phrase(StatusCode status_code) -> char *
  {
    switch (status_code)
    {
    case Response::StatusCode::Continue:
      return (char *)"Continue";
    case Response::StatusCode::Switching_Protocols:
      return (char *)"Switching Protocols";
    case Response::StatusCode::OK:
      return (char *)"OK";
    case Response::StatusCode::Created:
      return (char *)"Created";
    case Response::StatusCode::Accepted:
      return (char *)"Accepted";
    case Response::StatusCode::Non_Authoritative_Information:
      return (char *)"Non Authoritative Information";
    case Response::StatusCode::No_Content:
      return (char *)"No Content";
    case Response::StatusCode::Reset_Content:
      return (char *)"Reset Content";
    case Response::StatusCode::Partial_Content:
      return (char *)"Partial Content";
    case Response::StatusCode::Multiple_Choices:
      return (char *)"Multiple Choices";
    case Response::StatusCode::Moved_Permanently:
      return (char *)"Moved Permanently";
    case Response::StatusCode::Found:
      return (char *)"Found";
    case Response::StatusCode::See_Other:
      return (char *)"See Other";
    case Response::StatusCode::Not_Modified:
      return (char *)"Not Modified";
    case Response::StatusCode::Use_Proxy:
      return (char *)"Use Proxy";
    case Response::StatusCode::Temporary_Redirect:
      return (char *)"Temporary Redirect";
    case Response::StatusCode::Bad_Request:
      return (char *)"Bad Request";
    case Response::StatusCode::Unauthorized:
      return (char *)"Unauthorized";
    case Response::StatusCode::Payment_Required:
      return (char *)"Payment Required";
    case Response::StatusCode::Forbidden:
      return (char *)"Forbidden";
    case Response::StatusCode::Not_Found:
      return (char *)"Not Found";
    case Response::StatusCode::Method_Not_Allowed:
      return (char *)"Method Not Allowed";
    case Response::StatusCode::Not_Acceptable:
      return (char *)"Not Acceptable";
    case Response::StatusCode::Proxy_Authentication_Required:
      return (char *)"Proxy Authentication Required";
    case Response::StatusCode::Request_Timeout:
      return (char *)"Request Time-out";
    case Response::StatusCode::Conflict:
      return (char *)"Conflict";
    case Response::StatusCode::Gone:
      return (char *)"Gone";
    case Response::StatusCode::Length_Required:
      return (char *)"Length Required";
    case Response::StatusCode::Precondition_Failed:
      return (char *)"Precondition Failed";
    case Response::StatusCode::Request_Entity_Too_Large:
      return (char *)"Request Entity Too Large";
    case Response::StatusCode::Request_URI_Too_Large:
      return (char *)"Request URI Too Large";
    case Response::StatusCode::Unsupported_Media_Type:
      return (char *)"Unsurpported Media Type";
    case Response::StatusCode::Requested_Range_Not_Satisfiable:
      return (char *)"Requested Range Not Satisfiable";
    case Response::StatusCode::Expectation_Failed:
      return (char *)"Expectation Failed";
    case Response::StatusCode::Internal_Server_Error:
      return (char *)"Internal Sever Error";
    case Response::StatusCode::Not_Implemented:
      return (char *)"Not Implemented";
    case Response::StatusCode::Bad_Gateway:
      return (char *)"Bad Gateway";
    case Response::StatusCode::Service_Unavailable:
      return (char *)"Service Unavailable";
    case Response::StatusCode::Gateway_Timeout:
      return (char *)"Gateway Time-out";
    case Response::StatusCode::HTTP_Version_Not_Supported:
      return (char *)"HTTP Version Not Supported";
    default:
      return (char *)"";
    }
  }
};
}

#endif