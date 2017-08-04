#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include "Trie.h"

using namespace Http;

TEST_CASE("TrieNode", "[Trie]")
{
    Trie<std::string>::TrieNode n{};
    using node_ptr = Trie<std::string>::TrieNode;

    REQUIRE(n.data_ == "");
    REQUIRE(n.parent_ == nullptr);
    REQUIRE(n.child_.size() == 0);
}

TEST_CASE("Trie", "[Trie]")
{
    Trie<std::string> t{};
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

            auto node = t.root_->child_["pre"].get();
            REQUIRE(t.root_->child_.size() == 1);
            REQUIRE(node->child_.size() == 1);
            REQUIRE(node->data_ == "shortstr");
            REQUIRE(node->child_.at("suffix")->data_ == "longstr");
        }
    }

    SECTION("strings sharing common prefix")
    {
        /*
            should split 
            pre1         
            pre2  

            pre\
                |- 1        str1
                |- 2        str2
        */
        auto str1 = std::make_pair("pre1", "str1");
        auto str2 = std::make_pair("pre2", "str2");

        t.insert(str1);
        REQUIRE(t.root_->child_["pre1"]->data_ == str1.second);
        REQUIRE(t.root_->child_.size() == 1);

        SECTION("find")
        {
            node_ptr found;
            std::string suffix;
            std::tie(found, suffix) = t.find(str2);

            auto node = t.root_.get();
            REQUIRE(node == found);
            REQUIRE(suffix == "pre2");
        }

        SECTION("insert")
        {
            t.insert(str2);

            std::cout << t << std::endl;

            REQUIRE_THROWS_AS(t.root_->child_.at("pre1"), std::out_of_range);
            REQUIRE_THROWS_AS(t.root_->child_.at("pre2"), std::out_of_range);
            REQUIRE_NOTHROW(t.root_->child_.at("pre"));

            auto branch = t.root_->child_.at("pre").get();
            REQUIRE(branch->data_ == "");
            REQUIRE(branch->child_.size() == 2);
            REQUIRE_NOTHROW(branch->child_.at("1"));
            REQUIRE_NOTHROW(branch->child_.at("2"));
            REQUIRE_NOTHROW(branch->child_.at("1").get()->data_ == "str1");
            REQUIRE_NOTHROW(branch->child_.at("2").get()->data_ == "str2");
        }
    }
}