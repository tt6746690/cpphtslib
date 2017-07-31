#include <ostream>
#include <cassert>

#include "Request.h"
#include "Message.h"
#include "Utilities.h" // print pair

namespace Http
{

std::ostream &operator<<(std::ostream &strm, const Request &request)
{
    strm << request.method_ << " " << request.uri_ << " "
         << Message::version(request.version_major_, request.version_minor_) << std::endl;
    for (auto header : request.headers_)
    {
        strm << header << std::endl;
    }
    strm << "body: " << request.body_ << std::endl;

    return strm;
}
}