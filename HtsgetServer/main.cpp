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

using namespace asio;
using namespace Http;
using nlohmann::json;

int main()
{

    /* 
        curl --http1.1 -v -X GET '127.0.0.1:8888/reads/reads_id?format=BAM&referenceName=chr1&start=0&end=1000&fields=QNAME,FLAG,POS'
    */

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

        app->router_.get("/home", Handler([](Context &ctx) {
                             ctx.res_.write_text("<!doctype html><head></head><body><p> /home </p></body></html>");
                         }));

        app->router_.get("/reads/");
        app->router_.get("/reads/<id>",
                         Handler([](Context &ctx) {

                             auto foo = ctx.query_["foo"];
                             auto id = ctx.param_["id"];

                             std::vector<std::string> urls{
                                 "127.0.0.1:8888/data/1",
                                 "127.0.0.1:8888/data/2",
                                 "127.0.0.1:8888/data/3",
                                 "127.0.0.1:8888/data/4",
                             };

                             json_type res = {
                                 {"format", ctx.query_["format"]},
                                 {"urls", urls},
                                 {"md5", "md5_checksum_here"},
                             };

                             ctx.res_.write_json(res);
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