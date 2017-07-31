#ifndef SERVER_H
#define SERVER_H

/* Enforce C++11 support */
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#include "asio.hpp"
#endif

namespace Http
{

class GenericServer
{
public:
  using ServerAddr = std::pair<std::string, int>;

  /* non-copy-constructible */
  GenericServer(const GenericServer &) = delete;
  GenericServer &operator=(const GenericServer &) = delete;

  explicit GenericServer(const ServerAddr &server_address)
      : server_address_(server_address),
        io_service_(),
        acceptor_(io_service_),
        socket_(io_service_) {}

  /**
   * @brief   Starts the server
   *  Initiate io_service event loop, 
   *  acceptor instantiates and queues connection
   */
  void run();

  std::string host() const;
  int port() const;

private:
  const ServerAddr &server_address_;
  asio::io_service io_service_;
  asio::ip::tcp::acceptor acceptor_;
  asio::ip::tcp::socket socket_;

  // configs
  constexpr static int max_header_bytes = 1 << 20; // 1MB

  void accept_connection();
};
}

#endif