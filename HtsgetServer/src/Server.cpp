#include <iostream>
#include <memory>  // smart pointers
#include <utility> // make_pair
#include <chrono>

#include "Server.h"
#include "Connection.h"

namespace Htsget
{

void GenericServer::run()
{
    asio::ip::tcp::endpoint endpoint(
        asio::ip::tcp::v4(),
        port());

    // configure acceptor
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(
        asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.set_option(
        asio::ip::tcp::no_delay(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    /* accpeting connection on an event loop */
    accept_connection();
    io_service_.run();
}

void GenericServer::accept_connection()
{
    acceptor_.async_accept(
        socket_,
        [=](std::error_code ec) {
            if (!ec)
            {
                auto new_connection = std::make_shared<Connection>(std::move(socket_));
                new_connection->start();
            }
            accept_connection();
        });
}

std::string GenericServer::host() const
{
    return std::get<std::string>(server_address_);
}

int GenericServer::port() const
{
    return std::get<int>(server_address_);
}
}
