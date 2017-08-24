#ifndef SERVER_H
#define SERVER_H

#define ASIO_SEPARATE_COMPILATION

#define ASIO_STANDALONE

#undef ASIO_HEADER_ONLY

#include "asio.hpp"
#include "asio/impl/src.hpp"
#include "asio/ssl/impl/src.hpp"


#include "asio/ssl.hpp"

#include "Connection.h"
#include "Router.h"

#include <iostream>

namespace Http {

using ServerAddr = std::pair<std::string, int>;
using tcp_socket = asio::ip::tcp::socket;
using ssl_socket = asio::ssl::stream<asio::ip::tcp::socket>;


/**
 * @brief   A generic Http server
 */
template <typename Derived>
class GenericServer {
public:
  constexpr static int max_header_bytes = 1 << 20; // 1MB
public:
  /* non-copy-constructible */
  GenericServer(const GenericServer &) = delete;
  GenericServer &operator=(const GenericServer &) = delete;

  explicit GenericServer(const ServerAddr server_addr)
      : server_address_(server_addr), 
        io_service_(), 
        acceptor_(io_service_){};

  /**
   * @brief   Starts the server
   *  Initiate io_service event loop,
   *  acceptor instantiates and queues connection
   */
  void run(){
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port());

    // configure acceptor
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.set_option(asio::ip::tcp::no_delay(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    /* accpeting connection on an event loop */
    accept_connection();

    io_service_.run();
  }

  /**
   * @brief   Accept connection and creates new sessino 
   */
  void accept_connection(){
    acceptor_.async_accept(static_cast<Derived*>(this)->socket(), 
    [=](std::error_code ec) {
      if (!ec) {
        auto new_connection =
            std::make_shared<Connection>(std::move(
              static_cast<Derived*>(this)->socket_move()), router_);
        new_connection->start();
      }
      accept_connection();
    });
  }

  /**
   * @brief   Getting server address fields
   */
  std::string host() const{
    return std::get<std::string>(server_address_);
  }
  int port() const{
    return std::get<int>(server_address_); 
  }

public:
  Router<Handler> router_;
  ServerAddr server_address_; // (host, port) pair
  asio::io_service io_service_;
  asio::ip::tcp::acceptor acceptor_; // tcp acceptor
};


/**
 * @brief   An HTTP server 
 */
class HttpServer: public GenericServer<HttpServer>{
public:
  explicit HttpServer(const ServerAddr server_addr)
    : GenericServer(server_addr),
      socket_(io_service_) {};

  auto inline socket() -> tcp_socket& {
    return socket_;
  }
  auto inline socket_move() -> tcp_socket&& {
    return std::move(socket_);
  }

private:
  tcp_socket socket_;
};

/**
 * @brief   An HTTPS server
 */
class HttpsServer: public GenericServer<HttpsServer>{
public:
  explicit HttpsServer(const ServerAddr server_addr)
    : GenericServer(server_addr),
      context_(asio::ssl::context::sslv23),
      socket_(io_service_, context_)
      { 
        configure_ssl_context(); 
      };
    
  auto inline socket() -> ssl_socket&{
    return socket_;
  }
  auto inline socket_move() -> ssl_socket&& {
    return std::move(socket_);
  }
private:
  void configure_ssl_context();

private:
  asio::ssl::context context_;    
  ssl_socket socket_;
};
}

#endif