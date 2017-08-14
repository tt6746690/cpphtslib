#include <iostream>
#include <utility>
#include <memory>

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING // debugging
// #define NDEBUG

#include "Server.h"
#include "Uri.h"

using namespace std;
using namespace asio;
using namespace Http;

int main()
{

    try
    {
        io_service io;
        GenericServer::ServerAddr server_address = make_pair("127.0.0.1", 8888);
        auto app = make_unique<GenericServer>(server_address);

        /* url query parser middleware */
        app->router_.get("/reads/",
                         Handler([](Context &ctx) {
                             ctx.req_.query_ = Uri::make_query(ctx.req_.uri_.query_);
                         }));

        app->router_.get("/reads/<id>",
                         Handler([](Context &ctx) {

                             auto foo = ctx.query_["foo"];
                             auto id = ctx.param_["id"];

                             ctx.res_.write(
                                 "<!doctype html><head></head><body><p>query[foo]= " + foo + "  param[id]= " + id + "</p></body></html>");

                             ctx.res_.status_code(StatusCode::OK);
                             ctx.res_.content_type("text/html; charset=utf-8");

                             std::cout << ctx.res_ << std::endl;
                         }));

        std::cout << app->router_ << std::endl;
        app->run();
    }
    catch (std::exception e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}