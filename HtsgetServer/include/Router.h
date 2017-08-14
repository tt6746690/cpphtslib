#ifndef ROUTER_H
#define ROUTER_H

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include <tuple>

#include "Request.h"
#include "Response.h"
#include "Constants.h"
#include "Trie.h"

namespace Http
{

struct Context
{
    Request &req_;
    Response &res_;

    Context(Request &req, Response &res)
        : req_(req), res_(res){};
};

static int handler_count = 0;

class Handler
{
  public:
    using HandlerFunc = std::function<void(Context &)>;

    explicit Handler() : handler_(nullptr){};
    explicit Handler(HandlerFunc handler)
        : handler_(handler), handler_id_(handler_count) { ++handler_count; };

    operator bool() const { return handler_ != nullptr; }
    void operator()(Context &ctx) { handler_(ctx); }
    bool operator==(const Handler &rhs) { return handler_id_ == rhs.handler_id_; }
    bool operator!=(const Handler &rhs) { return !(operator==(rhs)); }

    HandlerFunc handler_;
    int handler_id_;

  public:
    friend auto inline operator<<(std::ostream &strm, Handler &handler) -> std::ostream &
    {
        return strm << "(" << handler.handler_id_ << ")";
    }
};

template <typename T>
class Router
{
  public:
    explicit Router()
        : routes_(method_count){};
    /**
     * @brief   Registers handler for provided method + path
     * 
     *  Rules
     *      -- specify named url parameter in angle brackets 
     *          -- /books/<id>
     * 
     * @precond path starts with /
     */
    auto handle(RequestMethod method, std::string path, T handler) -> void
    {
        assert(path.front() == '/');
        auto &route = routes_[etoint(method)];
        route.insert({path, handler});
    }

    template <typename Container = std::vector<RequestMethod>>
    auto handle(Container methods, std::string path, T handler) -> void
    {
        for (const auto &method : methods)
            handle(method, path, handler);
    }

    /**
     * @brief   Handle wrapper functions 
     */
    auto get(std::string path, T handler) -> void
    {
        handle(RequestMethod::GET, path, handler);
    }
    auto post(std::string path, T handler) -> void
    {
        handle(RequestMethod::POST, path, handler);
    }
    auto put(std::string path, T handler) -> void
    {
        handle(RequestMethod::PUT, path, handler);
    }
    auto use(std::string path, T handler) -> void
    {
        for (int method = static_cast<int>(RequestMethod::GET);
             method != static_cast<int>(RequestMethod::UNDETERMINED);
             ++method)
        {
            handle(static_cast<RequestMethod>(method), path, handler);
        }
    }
    /**
     * @brief   Resolve path to a sequence of handler calls
     *          If no matching path is found, the sequence is empty
     */
    auto resolve(RequestMethod method, std::string path) -> std::vector<T>
    {
        auto &route = routes_[etoint(method)];
        auto found = route.find(path);
        if (found == route.end())
            return {};
        std::vector<T> handle_sequence;

        while (found != route.end())
        {
            if (*found)
                handle_sequence.push_back(*found);
            --found;
        }

        std::reverse(handle_sequence.begin(), handle_sequence.end());
        return handle_sequence;
    }
    auto resolve(Request &req) -> std::vector<T>
    {
        auto method = req.method_;
        auto path = req.uri_.abs_path_;
        auto &route = routes_[etoint(method)];

        std::string param_key, param_value;
        auto found = route.find(path, param_key, param_value);

        if (found == route.end())
            return {};

        if (!param_key.empty() && !param_value.empty())
            req.param_.insert({param_key, param_value});

        std::vector<T> handle_sequence;

        while (found != route.end())
        {
            if (*found)
                handle_sequence.push_back(*found);
            --found;
        }

        std::reverse(handle_sequence.begin(), handle_sequence.end());
        return handle_sequence;
    }

  public:
    std::vector<Trie<T>> routes_;

  public:
    friend auto inline operator<<(std::ostream &strm, Router r) -> std::ostream &
    {
        for (int i = 0; i < method_count; ++i)
        {
            auto &trie = r.routes_[i];
            if (trie.size_ != 0)
            {
                strm << Request::request_method_to_string(static_cast<RequestMethod>(i)) << std::endl;
                strm << trie << std::endl;
            }
        }
        return strm;
    }
};
}

#endif