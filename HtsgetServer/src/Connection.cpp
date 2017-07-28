#include <iostream>
#include <iterator> // advance
#include <utility>  // enable_shared_from_this, move
#include "asio.hpp"

#include "Connection.h"
#include "RequestParser.h" // RequestParser::ParseStatus

using namespace std;

namespace Http
{

void Connection::start() { read_payload(); }

void Connection::terminate()
{
  socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
  socket_.close();
}

void Connection::read_payload()
{
  asio::async_read(socket_, asio::buffer(buffer_), asio::transfer_at_least(1), [
    this, self = shared_from_this()
  ](std::error_code ec, std::size_t bytes_transferred) {

    assert(this == self.get());

    if (!ec)
    {
      std::cout << std::string{buffer_.data(),
                               buffer_.data() + bytes_transferred};

      decltype(buffer_.begin()) begin;
      RequestParser::ParseStatus parse_status;

      std::tie(begin, parse_status) = request_parser_.parse(
          request_, buffer_.begin(), buffer_.begin() + bytes_transferred);

      std::cout << parse_status << std::endl;
    }
    else
    {
      return;
    }
  });
}
}
