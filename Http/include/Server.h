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
#include <memory>  
#include <utility> 
#include <chrono>



namespace Http {

using ServerAddr = std::pair<std::string, int>;

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
            std::make_shared<
              Connection<decltype(static_cast<Derived*>(this)->socket_)>>
              (std::move(static_cast<Derived*>(this)->socket_move()), router_);
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

  auto inline socket() -> TcpSocket& {
    return socket_;
  }
  auto inline socket_move() -> TcpSocket&& {
    return std::move(socket_);
  }

public:
  TcpSocket socket_;
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
    
  auto inline socket() -> SslSocket::lowest_layer_type&{
    return socket_.lowest_layer();
  }
  auto inline socket_move() -> SslSocket&& {
    return std::move(socket_);
  }
private:
  void inline configure_ssl_context(){
    context_.set_options(asio::ssl::context::default_workarounds |
                          asio::ssl::context::no_sslv2 |
                          asio::ssl::context::single_dh_use);

    // https://stackoverflow.com/questions/6452756/exception-running-boost-asio-ssl-example

    context_.set_password_callback(
        [](std::size_t max_length, asio::ssl::context::password_purpose
        purpose) {
          return "password";
        });
    context_.use_private_key_file("Http/ssl/server.key", asio::ssl::context::pem);
    context_.use_certificate_chain_file("Http/ssl/server.crt");
    context_.use_tmp_dh_file("Http/ssl/dh512.pem");
  };

public:
  asio::ssl::context context_;    
  SslSocket socket_;
};
}

#endif