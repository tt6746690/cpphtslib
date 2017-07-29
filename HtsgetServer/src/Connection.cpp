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
  asio::async_read(
      socket_,
      asio::buffer(buffer_),
      asio::transfer_at_least(1),
      [ this, self = shared_from_this() ](std::error_code ec, std::size_t bytes_read) {

        assert(this == self.get());

        if (!ec)
        {

#ifndef NDEBUG
          std::cout << std::string{buffer_.data(),
                                   buffer_.data() + bytes_read};
#endif

          decltype(buffer_.begin()) begin;
          RequestParser::ParseStatus parse_status;

          std::tie(begin, parse_status) = request_parser_.parse(
              request_, buffer_.begin(), buffer_.begin() + bytes_read);

          /**
       * Current buffer is fully read, branch on ParseStatus
       *    -- in_progress, 
       *        so continue do async read 
       *    -- accept,
       *        request header parsing finished
       *    -- reject,
       *        request has malformed syntax, send 400
       */
          switch (parse_status)
          {
          case RequestParser::ParseStatus::in_progress:
            read_payload();
            return;
          case RequestParser::ParseStatus::accept:
            // start the route...
            write_payload();
          case RequestParser::ParseStatus::reject:
            // sets 400 to response
            write_payload();
          default:
            return;
          }
        }
      });
}

void Connection::write_payload()
{
  asio::async_write(
      socket_,
      asio::buffer(response_.payload_), asio::transfer_all(),
      [ this, self = shared_from_this() ](std::error_code ec, std::size_t bytes_written) {
        if (!ec)
        {
          terminate();
        }
        // connection shared_ptr goes out of scope, Connection lifetype ends here
      });
}
}
