#include <iostream>
#include <memory>  // smart pointers
#include <utility> // make_pair
#include <chrono>

#include "Server.h"

namespace Htsget
{

void GenericServer::run()
{
    configure_acceptor();

    acceptor_.async_accept(
        socket_,
        [=](auto ec) {
            std::cout << "accepting connection" << std::endl;
        });

    /* start event loop */
    io_service_.run();
}

void GenericServer::configure_acceptor()
{
    asio::ip::tcp::endpoint endpoint(
        asio::ip::tcp::v4(),
        std::get<int>(server_address_));

    // listen on tcp port
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(
        asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.set_option(
        asio::ip::tcp::no_delay(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
}
}
