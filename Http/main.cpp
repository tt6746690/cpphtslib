#include "json.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING // debugging
// #define NDEBUG

#include "Constants.h"
#include "Response.h"
#include "Server.h"
#include "Uri.h"
#include "Utilities.h"
#include "Cors.h"

#include <cstdio>
#include <cstdlib>

using namespace asio;
using namespace Http;
using nlohmann::json;

int main() {

  try {
    io_service io;
    GenericServer::ServerAddr server_address =
        std::make_pair("127.0.0.1", 8888);
    auto app = std::make_unique<GenericServer>(server_address);

    /*  Cors middleware on all routes

    curl --http1.1 -v -X GET -H "Origin: localhost" '127.0.0.1:8888/r'

    curl --http1.1 -v -X OPTIONS -H " Access-Control-Request-Method: GET" '127.0.0.1:8888/r'

    curl --http1.1 -v -X OPTIONS -H "Origin: localhost" -H "Access-Control-Request-Method: GET" '127.0.0.1:8888/'
    */
    app->router_.use("/", Cors({"*"}, {RequestMethod::GET}, 51840000));

    /* url query middleware */
    app->router_.get("/r", Handler([](Context &ctx) {
                       // url query parser
                       ctx.req_.query_ = Uri::make_query(ctx.req_.uri_.query_);

                       json_type urlparse = {
                           {"query", ctx.query_}, {"param", ctx.param_},
                       };
                       std::cout << std::setw(4) << urlparse << std::endl;
                     }));

    std::cout << app->router_ << std::endl;
    app->run();
  } catch (std::exception e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
