#include "catch.hpp"
#include <utility>

#include "Trie.h"

using namespace Http;

TEST_CASE("TrieNode", "[Trie]")
{
    Trie<std::string>::TrieNode n;
    using node_ptr = Trie<std::string>::TrieNode;

    REQUIRE(n.data_ == "");
    REQUIRE(n.parent_ == nullptr);
    REQUIRE(n.child_.size() == 0);
}

TEST_CASE("Trie", "[Trie]")
{
    Trie<std::string> t;
    using node_ptr = Trie<std::string>::TrieNode *;

    REQUIRE(t.size_ == 0);
    REQUIRE(t.root_->data_ == "/");
    REQUIRE(t.root_->parent_ == nullptr);
    REQUIRE(t.root_->child_.size() == 0);

    SECTION("2 different string")
    {
        t.insert(std::make_pair("a", "value1"));
        t.insert(std::make_pair("b", "value2"));

        REQUIRE(t.root_->child_["a"]->data_ == "value1");
        REQUIRE(t.root_->child_["b"]->data_ == "value2");
        REQUIRE(t.root_->child_.size() == 2);
    }

    SECTION("substring")
    {
        /*
            pre         [shortstr]
            presuffix   [longstr]
        */
        auto pre = std::make_pair("pre", "shortstr");
        t.insert(pre);
        REQUIRE(t.root_->child_["pre"]->data_ == pre.second);
        REQUIRE(t.root_->child_.size() == 1);

        auto presuffix = std::make_pair("presuffix", "longstr");

        SECTION("find")
        {
            node_ptr found;
            std::string suffix;
            std::tie(found, suffix) = t.find(presuffix);

            auto node = t.root_->child_["pre"].get();
            REQUIRE(node == found);
            REQUIRE(suffix == "suffix");
        }

        SECTION("insert")
        {
            t.insert(presuffix);

            REQUIRE(t.root_->child_.size() == 1);
            REQUIRE(t.root_->child_["pre"]->child_.size() == 1);
            REQUIRE(t.root_->child_["pre"]->data_ == "shortstr");
            REQUIRE(t.root_->child_["pre"]->child_.at("suffix")->data_ == "longstr");
            std::cout << t << std::endl;
        }
    }
}
