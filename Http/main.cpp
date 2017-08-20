#include "json.hpp"

#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <string>

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING // debugging
// #define NDEBUG

#include "Server.h"
#include "Uri.h"
#include "Utilities.h"
#include "Response.h"

#include <cstdlib>
#include <cstdio>

using namespace asio;
using namespace Http;
using nlohmann::json;

int main()
{

    try
    {
        io_service io;
        GenericServer::ServerAddr server_address = std::make_pair("127.0.0.1", 8888);
        auto app = std::make_unique<GenericServer>(server_address);

        /* url query middleware */
        app->router_.get("/",
                         Handler([](Context &ctx) {
                             ctx.req_.query_ = Uri::make_query(ctx.req_.uri_.query_);

                             json_type urlparse = {
                                 {"query", ctx.query_},
                                 {"param", ctx.param_},
                             };
                             std::cout << std::setw(4) << urlparse << std::endl;
                         }));

        std::cout << app->router_ << std::endl;
        app->run();
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
