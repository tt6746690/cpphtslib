#include <utility> // enable_shared_from_this, move
#include "asio.hpp"

#include "Connection.h"

namespace Htsget
{

void Connection::start()
{
    read_payload();
}

void Connection::terminate()
{
    socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
    socket_.close();
}

void Connection::read_payload()
{
}
}