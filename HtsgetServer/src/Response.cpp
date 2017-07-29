#include <ostream>

#include "Response.h"
#include "Utilities.h" // print pair

namespace Http
{

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