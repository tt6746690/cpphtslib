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
          decltype(buffer_.begin()) begin;
          ParseStatus parse_status;

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
          case ParseStatus::in_progress:
          {
            read_payload();
            break;
          }
          case ParseStatus::accept:
          {
            response_.status_code(StatusCode::OK);
            response_.version_major_ = request_.version_major_;
            response_.version_minor_ = request_.version_minor_;

            auto handlers = router_.resolve(request_);
            for (auto &handler : handlers)
            {
              handler(context_);
            }

            write_payload();
            break;
          }

          case ParseStatus::reject:
          {
            response_.status_code(StatusCode::Bad_Request);
            write_payload();
            break;
          }
          default:
            break;
          }
        }
      });
}

void Connection::write_payload()
{
  asio::async_write(
      socket_,
      asio::buffer(response_.to_payload()),
      asio::transfer_all(),
      [ this, self = shared_from_this() ](std::error_code ec, std::size_t bytes_written) {
        if (!ec)
        {
          terminate();
        }
      });
}
}
