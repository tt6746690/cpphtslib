#ifndef SERVER_H
#define SERVER_H

/* Enforce C++11 support */
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#include "asio.hpp"
#endif

#include "asio/ssl.hpp"

#include "Router.h"

namespace Http {

class GenericServer {
public:
  using ServerAddr = std::pair<std::string, int>;

  /* non-copy-constructible */
  GenericServer(const GenericServer &) = delete;
  GenericServer &operator=(const GenericServer &) = delete;

  explicit GenericServer(const ServerAddr server_addr)
      : server_address_(server_addr), io_service_(), acceptor_(io_service_), socket_(io_service_){};

  /**
   * @brief   Starts the server
   *  Initiate io_service event loop,
   *  acceptor instantiates and queues connection
   */
  void run();

  std::string host() const;
  int port() const;


public:
  Router<Handler> router_;

private:
  ServerAddr server_address_;   // (host, port) pair
  asio::io_service io_service_;   
  asio::ip::tcp::acceptor acceptor_;    // tcp acceptor
  asio::ip::tcp::socket socket_;
  
  // configs
  constexpr static int max_header_bytes = 1 << 20; // 1MB

  void accept_connection();
};

class HttpsServer: public GenericServer {
public:
  HttpsServer(const ServerAddr server_addr)
    : GenericServer(server_addr), context_(asio::ssl::context::sslv23){
      configure_ssl_context();
    }
private:
  void configure_ssl_context();

private:
  asio::ssl::context context_;    // ssl
};



}

#endif