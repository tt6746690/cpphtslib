#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <utility>
#include <bitset>
#include <vector>
#include <cstddef>
#include <iterator>
#include <unordered_map>
#include <array>
#include <memory>
#include <ostream>

#include <sparsepp/spp.h>

#include <Utilities.h>

namespace Http
{

constexpr size_t uri_charset_size = constexpr_strlen(uri_charset);
constexpr size_t radix = 16;

template <typename T>
class Trie
{
public:
  struct TrieNode;

  using key_type = std::string;
  using mapped_type = T;
  using value_type = std::pair<const key_type, T>;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = TrieNode *;
  using const_iterator = const TrieNode *;

  using node_ptr = TrieNode *;
  using uniq_node_ptr = std::unique_ptr<TrieNode>;

public:
  /**
 * @brief   Node of a Trie
 * 
 * @param   data_       auxiliary data 
 *                      internal node:  may or may not contain data_
 *                      leaf node: must has valid data_
 * @param   parent_     pointer to parent node, nullptr for root_
 * @param   child_      a mapping of prefixes and pointers to associated nodes 
 */
  struct TrieNode
  {
    using node_ptr = TrieNode *;
    using uniq_node_ptr = std::unique_ptr<TrieNode>;

    T data_;
    node_ptr parent_ = nullptr;
    std::unordered_map<std::string, uniq_node_ptr> child_;

    TrieNode(T data = T(), node_ptr parent = nullptr)
        : data_(data), parent_(parent){};
  };

  Trie()
      : root_(std::make_unique<TrieNode>("/")), size_(0){};

  /**
   * @brief   Find a TrieNode in Trie rooted at node 
   *          with key equivalent to key, If no exact match, 
   *          Return the last node with a common prefix
   */
  auto static find_in(std::string &key, node_ptr node) -> node_ptr
  {
    if (!key.size())
      return node;

    for (int i = 1; i < key.size(); i++)
    {
      auto found = node->child_.find(key.substr(0, i));
      if (found != node->child_.end())
      {
        key = key.substr(i);
        return find_in(key, (found->second).get());
      }
    }
    return node;
  }
  auto find(const value_type &value) -> std::pair<node_ptr, std::string>
  {
    std::string key{value.first};
    return std::make_pair(find_in(key, root_.get()), key);
  }

  /**
   * @brief   Inserts data to Trie given key 
   */
  auto insert(const value_type &value) -> node_ptr
  {
    if (!root_->child_.size())
    {
      root_->child_.emplace(
          std::make_pair(value.first, newNode(value, root_.get())));
      return root_.get();
    }

    node_ptr node;
    std::string suffix;
    std::tie(node, suffix) = find(value);

    node->child_.emplace(
        std::make_pair(suffix, newNode(value, node)));

    return node;
  }

  auto newNode(const value_type &value, node_ptr parent) -> uniq_node_ptr
  {
    return std::make_unique<TrieNode>(value.second, parent);
  }

public:
  uniq_node_ptr root_;
  size_t size_ = 0;

public:
  friend inline auto operator<<(std::ostream &strm, Trie &t) -> std::ostream &
  {
    strm << t.root_->data_ << std::endl;
    return strm << *t.root_ << std::endl;
  }
  friend inline auto operator<<(std::ostream &strm, TrieNode &node) -> std::ostream &
  {
    for (auto &child : node.child_)
    {
      strm << "|-" << child.first << "\\"
           << "\t\t" << child.second->data_ << std::endl;
      if (child.second->child_.size())
        strm << " " << *child.second.get() << std::endl;
    }
    return strm;
  }
};
}
#endif