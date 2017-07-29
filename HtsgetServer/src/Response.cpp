#include <ostream>
#include <string>

#include "Response.h"
#include "Utilities.h" // print pair, etostr

namespace Http
{

std::string Response::status_line() const
{
    return "HTTP" + std::to_string(version_major_) + "." + std::to_string(version_minor_) + " " + etostr(status_code_) + " " + reason_ + "\r\n";
};

std::string Response::status_line(
    StatusCode status_code,
    std::string reason,
    std::string version_major,
    std::string version_minor)
{
    return "HTTP" + version_major + "." + version_minor + " " + etostr(status_code) + " " + reason + "\r\n";
}

std::ostream &operator<<(std::ostream &strm, const Response &response)
{
    strm << "Status  : " << response.status_code_ << std::endl
         << "Reason  : " << response.reason_ << std::endl
         << "Version : " << response.version_major_ << "." << response.version_minor_ << std::endl
         << "Headers : " << std::endl;

    for (auto header : response.headers_)
    {
        strm << "\t\t" << header << std::endl;
    }

    strm << "Body    : " << response.body_ << std::endl;

    return strm;
}
}