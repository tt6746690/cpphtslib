#ifndef CONNECTION_H
#define CONNECTION_H

#include "asio.hpp"
#include "asio/ssl.hpp"

#include <type_traits>

#include "Request.h"
#include "RequestParser.h"
#include "Response.h"
#include "Router.h"

namespace Http {

using TcpSocket = asio::ip::tcp::socket;
using SslSocket = asio::ssl::stream<asio::ip::tcp::socket>;


template <typename SocketType>
class Connection : public std::enable_shared_from_this<Connection<SocketType>> {
public:
  explicit Connection(
    SocketType&& socket, Router<Handler> &router)
      : socket_(std::move(socket)), 
        router_(router){};

public:

  /**
   * @brief   Starts reading asynchronously
   *          For TLS, do handshake first
   */
  void start();

  /**
   * @brief   Shutdown socket 
   */
  void terminate();

  /**
   * @brief   Read some from socket and save to buffer
   *          Parses request and executes handlers
   */
  void read_payload();

  /**
   * @brief   Write buffer to socket 
   *          Call terminate()
   */
  void write_payload();

private:
  SocketType socket_;
  std::array<char, 4096> buffer_;

  Request request_;
  Response response_;
  Context context_{request_, response_};
  RequestParser request_parser_;
  Router<Handler> &router_;
};



}

#endif