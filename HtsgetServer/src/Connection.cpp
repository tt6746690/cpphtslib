#include <iostream>
#include <iterator> // advance
#include <utility>  // enable_shared_from_this, move
#include "asio.hpp"

#include "Connection.h"
#include "RequestParser.h"
#include "Constants.h"

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

          // #ifndef NDEBUG
          std::cout << std::string{buffer_.data(),
                                   buffer_.data() + bytes_read};
          // #endif

          decltype(buffer_.begin()) begin;
          ParseStatus parse_status;

          std::tie(begin, parse_status) = request_parser_.parse(
              request_, buffer_.begin(), buffer_.begin() + bytes_read);

          std::cout << request_ << std::endl;

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
          case ParseStatus::in_progress:
            read_payload();
            return;
          case ParseStatus::accept:
            // handler start the roure.. for now just return statusline
            response_.status_code(StatusCode::OK);
            write_payload();
            return;
          case ParseStatus::reject:
            response_.status_code(StatusCode::Not_Found);
            write_payload();
            return;
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
      asio::buffer(response_.to_payload()), asio::transfer_all(),
      [ this, self = shared_from_this() ](std::error_code ec, std::size_t bytes_written) {
        if (!ec)
        {
          terminate();
        }
      });
}
}
