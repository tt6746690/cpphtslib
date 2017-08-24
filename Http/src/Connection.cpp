#include "asio.hpp"
#include "asio/ssl.hpp"

#include <iostream>
#include <iterator> // advance
#include <utility>  // enable_shared_from_this, move

#include "Connection.h"
#include "Uri.h"
#include "Constants.h"

using namespace std;

namespace Http {


template<typename SocketType>
void Connection<SocketType>::start() {}


template<>
void Connection<TcpSocket>::start() { read_payload(); }

template<>
void Connection<SslSocket>::start(){
   socket_.async_handshake(asio::ssl::stream_base::server,
        [this, self=this->shared_from_this()]
          (std::error_code ec){
            read_payload();
          });
}


template<typename SocketType>
void Connection<SocketType>::terminate() {};

template<>
void Connection<TcpSocket>::terminate(){
  socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
  socket_.close();
}

template<>
void Connection<SslSocket>::terminate(){
  socket_.shutdown();
}


template<typename SocketType>
void Connection<SocketType>::read_payload() {

  asio::async_read(
    socket_, 
    asio::buffer(buffer_), 
    asio::transfer_at_least(1),
    [ this, self = this->shared_from_this() ]
      (std::error_code ec, std::size_t bytes_read) {

      assert(this == self.get());

      if (!ec) {
        decltype(buffer_.begin()) begin;
        ParseStatus parse_status;

        std::tie(begin, parse_status) =
            request_parser_.parse(request_, buffer_.begin(),
                                  buffer_.begin() + bytes_read);

        /**
        * Current buffer is fully read, branch on ParseStatus
        *    -- in_progress,
        *        so continue do async read
        *    -- accept,
        *        request header parsing finished
        *    -- reject,
        *        request has malformed syntax, send 400
        */
        switch (parse_status) {
        case ParseStatus::in_progress: {
          read_payload();
          break;
        }
        case ParseStatus::accept: {
          response_.status_code(StatusCode::OK);
          response_.version_major_ = request_.version_major_;
          response_.version_minor_ = request_.version_minor_;
          request_.query_ = Uri::make_query(request_.uri_.query_);

          auto handlers = router_.resolve(request_);
          for (auto &handler : handlers) {
            handler(context_);
          }

          write_payload();
          break;
        }

        case ParseStatus::reject: {
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

template<typename SocketType>
void Connection<SocketType>::write_payload() {
  asio::async_write(
    socket_, 
    asio::buffer(response_.to_payload()),
    asio::transfer_all(),
    [ this, self = this->shared_from_this() ](
        std::error_code ec, std::size_t bytes_written) {
      if (!ec) {
        terminate();
      }
    });
}



}
