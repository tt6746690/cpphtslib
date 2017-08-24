#include <chrono>
#include <iostream>
#include <memory>  // smart pointers
#include <utility> // make_pair

#include "asio.hpp"

#include "Connection.h"
#include "Server.h"

namespace Http {

void GenericServer::run() {
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

void GenericServer::accept_connection() {
  acceptor_.async_accept(socket_, [=](std::error_code ec) {
    if (!ec) {
      auto new_connection =
          std::make_shared<Connection>(std::move(socket_), router_);
      new_connection->start();
    }
    accept_connection();
  });
}

std::string GenericServer::host() const {
  return std::get<std::string>(server_address_);
}

int GenericServer::port() const { return std::get<int>(server_address_); }

void HttpsServer::configure_ssl_context() {
  context_.set_options(asio::ssl::context::default_workarounds |
                       asio::ssl::context::no_sslv2 |
                       asio::ssl::context::single_dh_use);

  // https://stackoverflow.com/questions/6452756/exception-running-boost-asio-ssl-example

  context_.set_password_callback(
      [](std::size_t max_length, asio::ssl::context::password_purpose
      purpose) {
        return "password";
      });
  context_.use_certificate_chain_file("Http/ssl/server.crt");
  context_.use_private_key_file("Http/ssl/server.key",
  asio::ssl::context::pem);
  context_.use_tmp_dh_file("Http/ssl/dh512.pem");
}
}
