#ifndef ROUTER_H
#define ROUTER_H

#include <functional>
#include <string>
#include <vector>

#include "Request.h"
#include "Response.h"
#include "Constants.h"
#include "Trie.h"

#include <iostream>

namespace Http
{

struct Context
{
    Request &req_;
    Response &res_;
};

using Handler = std::function<void(Context)>;

template <typename X = Handler>
class Router
{
  public:
    explicit Router()
        : tries_(method_count){};
    /**
     * @brief   Registers handler for provided method + path
     * 
     * @precond path starts with /
     */
    auto handle(RequestMethod method, std::string path, X handler) -> void
    {
        assert(path.front() == '/');
        auto trie = tries_[etoint(method)];
        trie.insert(path, handler);
    }

    /**
     * @brief   Resolve path to a sequence of handler calls
     * 
     *          If no matching path is found, the sequence is empty
     */
    auto resolve(RequestMethod method, std::string path) -> std::vector<X>
    {
        auto trie = tries_[etoint(method)];
        auto found = trie.find(path);

        if (found == trie.end())
            return {};

        std::vector<X> handle_sequence;

        while (found != trie.end())
        {
            if (*found)
                handle_sequence.push_back(*found);
            --found;
        }
        return handle_sequence;
    }

  public:
    std::vector<Trie<X>> tries_;
};
}

#endif