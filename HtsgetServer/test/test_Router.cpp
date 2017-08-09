#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include "Trie.h"
#include "Router.h"
#include "Constants.h"

using namespace Http;

TEST_CASE("handle, resolves", "[Router]")
{

    Response res;
    Request req;
    Context ctx(req, res);

    Router<Handler> r{};
    REQUIRE(r.routes_.size() == method_count);
    REQUIRE(r.routes_[0].size() == 0);
    REQUIRE(!r.routes_.front().root_->data_);

    SECTION("handle")
    {

        r.handle(RequestMethod::GET, "/home", Handler([](Context &ctx) {
                     std::cout << "Handler: GET/home" << std::endl;
                 }));

        auto home_foo_handle = Handler([](Context &ctx) {
            std::cout << "Handler: GET/home/foo" << std::endl;
        });
        r.handle(RequestMethod::GET, "/home/foo", home_foo_handle);

        auto home_bar_handle = Handler([](Context &ctx) {
            std::cout << "Handler: GET/home/bar" << std::endl;
        });
        r.handle(RequestMethod::GET, "/home/bar", home_bar_handle);

        REQUIRE(r.routes_[0].size() == 3);
        REQUIRE(home_foo_handle.handler_id_ == 1);
        REQUIRE(home_bar_handle.handler_id_ == 2);

        SECTION("resolve")
        {
            auto handles = r.resolve(RequestMethod::GET, "/home");
            REQUIRE(handles.size() == 1);

            auto foo_handles = r.resolve(RequestMethod::GET, "/home/foo");
            auto bar_handles = r.resolve(RequestMethod::GET, "/home/bar");
            REQUIRE(foo_handles.size() == 2);
            REQUIRE(bar_handles.size() == 2);
            REQUIRE(foo_handles.front() == bar_handles.front());
            REQUIRE(foo_handles.back() != bar_handles.back());

            handles = r.resolve(RequestMethod::POST, "/home/bar");
            REQUIRE(handles.size() == 0);
        }
    }
}