#include <ostream>

#include "Request.h"
#include "Utilities.h" // print pair

namespace Http
{

std::ostream &operator<<(std::ostream &strm, const Request &request)
{
    strm << "Method  : " << request.method_ << std::endl
         << "Uri     : " << request.uri_ << std::endl
         << "Version : " << request.version_major_ << "." << request.version_minor_ << std::endl
         << "Headers : " << std::endl;

    for (auto header : request.headers_)
    {
        strm << "\t\t" << header << std::endl;
    }

    strm << "Body    : " << std::endl;

    return strm;
}
}