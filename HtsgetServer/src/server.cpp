#include <iostream>
#include <memory>  // smart pointers
#include <utility> // make_pair
#include <chrono>

#include "server.h"

using namespace std;

namespace Htsget
{

void GenericServer::run()
{
    configure_acceptor();

    acceptor_.async_accept(
        socket_,
        [=](auto ec) {
            cout << "accepting connection" << endl;
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
    acceptor_.bind(endpoint);
    acceptor_.listen();
}
}

int main()
{
    using Htsget::GenericServer;

    asio::io_service io;
    GenericServer::ServerAddr server_address = make_pair("127.0.0.1", 8888);

    try
    {
        auto server = make_unique<GenericServer>(server_address);
        server->run();
    }
    catch (std::exception e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}