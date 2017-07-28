#ifndef CONNECTION_H
#define CONNECTION_H

#include "Request.h"
#include "Response.h"
#include "RequestParser.h"

namespace Http
{

class Connection
    : public std::enable_shared_from_this<Connection>
{
public:
  explicit Connection(asio::ip::tcp::socket socket)
      : socket_(std::move(socket)){};

  void start();
  void terminate();
  void read_payload();
  void write_payload();

private:
  asio::ip::tcp::socket socket_;
  std::array<char, 4096> buffer_;

  Request request_;
  Response response_;
  RequestParser request_parser_;
};
}

#endif