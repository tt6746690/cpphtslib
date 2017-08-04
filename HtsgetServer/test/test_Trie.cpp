#include "catch.hpp"
#include <utility>
#include <stdexcept>

#include "Trie.h"

using namespace Http;

TEST_CASE("TrieNode", "[Trie]")
{
    Trie<std::string>::TrieNode n{};

    REQUIRE(n.data_ == "");
    REQUIRE(n.parent_ == nullptr);
    REQUIRE(n.child_.size() == 0);
}

TEST_CASE("Trie Full example", "[Trie]")
{
    Trie<std::string> t{};

    SECTION("insert a lot")
    {
        t.insert({"smile", "smile"});
        t.insert({"smiled", "smiled"});
        t.insert({"zzz", "zzz"});
        t.insert({"smiles", "smiles"});
        t.insert({"smiling", "smiling"});

        std::cout << t << std::endl;
    }
}

TEST_CASE("Trie::{find, insert}", "[Trie]")
{
    Trie<std::string> t{};
    using node_ptr = Trie<std::string>::TrieNode *;

    REQUIRE(t.size_ == 0);
    REQUIRE(t.root_->data_ == "/");
    REQUIRE(t.root_->parent_ == nullptr);
    REQUIRE(t.root_->child_.size() == 0);

    SECTION("different strings")
    {
        t.insert(std::make_pair("a", "value1"));
        t.insert(std::make_pair("b", "value2"));

        REQUIRE(t.root_->child_["a"]->data_ == "value1");
        REQUIRE(t.root_->child_["b"]->data_ == "value2");
        REQUIRE(t.root_->child_.size() == 2);
    }

    SECTION("previous key is substring")
    {
        /*
            pre         
            presuffix   

            should append new key 

            pre\
                |- suffix
        */
        auto prev_key = std::make_pair("pre", "shortstr");
        auto new_key = std::make_pair("presuffix", "longstr");

        t.insert(prev_key);
        REQUIRE(t.root_->child_["pre"]->data_ == prev_key.second);
        REQUIRE(t.root_->child_.size() == 1);

        SECTION("find")
        {
            node_ptr found;
            std::string suffix;
            std::tie(found, suffix) = t.find(new_key);

            auto node = t.root_->child_["pre"].get();
            REQUIRE(node == found);
            REQUIRE(suffix == "suffix");
        }

        SECTION("insert")
        {
            t.insert(new_key);

            auto node = t.root_->child_["pre"].get();
            REQUIRE(t.root_->child_.size() == 1);
            REQUIRE(node->child_.size() == 1);
            REQUIRE(node->data_ == "shortstr");
            REQUIRE(node->child_.at("suffix")->data_ == "longstr");
        }
    }

    SECTION("new key is substring")
    {
        /*
            presuffix         
            pre

            pre\
                |- suffix
        */
        auto prev_key = std::make_pair("presuffix", "prev_key");
        auto new_key = std::make_pair("pre", "new_key");

        t.insert(prev_key);

        REQUIRE(t.root_->child_["presuffix"]->data_ == prev_key.second);
        REQUIRE(t.root_->child_.size() == 1);

        SECTION("find")
        {
            node_ptr found;
            std::string suffix;
            std::tie(found, suffix) = t.find(new_key);

            auto node = t.root_.get();
            REQUIRE(node == found);
            REQUIRE(suffix == "pre");
        }

        SECTION("insert")
        {
            t.insert(new_key);
            REQUIRE_THROWS_AS(t.root_->child_.at("presuffix"), std::out_of_range);
            REQUIRE_NOTHROW(t.root_->child_.at("pre"));

            auto branch = t.root_->child_.at("pre").get();
            REQUIRE(branch->data_ == new_key.second);
            REQUIRE(branch->child_.size() == 1);
            REQUIRE_NOTHROW(branch->child_.at("suffix"));
            REQUIRE(branch->child_.at("suffix").get()->data_ == prev_key.second);
        }
    }

    SECTION("keys share common prefix")
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

            REQUIRE_THROWS_AS(t.root_->child_.at("pre1"), std::out_of_range);
            REQUIRE_THROWS_AS(t.root_->child_.at("pre2"), std::out_of_range);
            REQUIRE_NOTHROW(t.root_->child_.at("pre"));

            auto branch = t.root_->child_.at("pre").get();
            REQUIRE(branch->data_ == "");
            REQUIRE(branch->child_.size() == 2);
            REQUIRE_NOTHROW(branch->child_.at("1"));
            REQUIRE_NOTHROW(branch->child_.at("2"));
            REQUIRE(branch->child_.at("1").get()->data_ == "str1");
            REQUIRE(branch->child_.at("2").get()->data_ == "str2");
        }
    }
}