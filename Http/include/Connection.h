#ifndef CONNECTION_H
#define CONNECTION_H

#include "Request.h"
#include "RequestParser.h"
#include "Response.h"
#include "Router.h"

namespace Http {

class Connection : public std::enable_shared_from_this<Connection> {
public:
  explicit Connection(asio::ip::tcp::socket socket, Router<Handler> &router)
      : socket_(std::move(socket)), router_(router){};

  void start();
  void terminate();
  void read_payload();
  void write_payload();

private:
  asio::ip::tcp::socket socket_;
  std::array<char, 4096> buffer_;

  Request request_;
  Response response_;
  Context context_{request_, response_};
  RequestParser request_parser_;
  Router<Handler> &router_;
};
}

#endif