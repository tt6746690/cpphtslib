#include <ostream>
#include <string>
#include <utility>

#include "Response.h"
#include "Utilities.h" // print pair, etostr

namespace Http
{

auto Response::to_payload() const -> std::string
{
    return status_line();
}

StatusCode Response::status_code()
{
    return status_code_;
}

void Response::status_code(StatusCode status_code)
{
    status_code_ = status_code;
}

std::string Response::reason_phrase()
{
    return status_code_to_reason(status_code_);
}

auto Response::status_line() const -> std::string
{
    return to_status_line(status_code_, version_major_, version_minor_);
};

auto Response::to_status_line(
    StatusCode status_code,
    int http_version_major,
    int http_version_minor) -> std::string
{
    return "HTTP/" + version(http_version_major, http_version_minor) + " " +
           std::to_string(status_code_to_int(status_code)) + " " +
           status_code_to_reason(status_code) + "\r\n";
}

std::ostream &
operator<<(std::ostream &strm, const Response &response)
{
    strm << response.status_line() << std::endl;
    for (auto &header : response.headers_)
    {
        strm << "\t\t" << header << std::endl;
    }
    return strm << "Body    : " << response.body_ << std::endl;
}
}