#include <iostream>
#include <utility>
#include <memory>

#include "server.h"

using namespace std;

int main()
{
    try
    {
        using Htsget::GenericServer;

        asio::io_service io;
        GenericServer::ServerAddr server_address = make_pair("127.0.0.1", 8888);

        auto server = make_unique<GenericServer>(server_address);
        server->run();
    }
    catch (std::exception e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}