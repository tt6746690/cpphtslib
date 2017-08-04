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
#include <algorithm>

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

    explicit TrieNode(){};
    explicit TrieNode(node_ptr parent, T data = T())
        : data_(data), parent_(parent){};
  };

  Trie()
      : root_(std::make_unique<TrieNode>(nullptr, "/")), size_(0){};

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

  /** 
   * @brief   Given a value = (Key,T), find the node to insert to
   * 
   * invariant 
   *  suffix.substr(0, i) is not contained in node's map
   *  i.e. suffix must be inserted in node's map in some way
   */
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
          std::make_pair(value.first, newNode(root_.get(), value)));
      return root_.get();
    }

    node_ptr node;
    std::string input_key;
    std::tie(node, input_key) = find(value);

    /**
     * @note  keys in map has unique char at index 0
     *        if such previous key exists, we have a common prefix, 
     *        a duplicate first char indicates a need to split the edge
     *  -- determine a common prefix 
     *  -- removes previous key->node from child_ map
     *  -- Either the branching node 
     *      -- holds data, 
     *      -- does not hold data
     *  -- transfer ownership of previous node to the branching node
     * 
     *        Otherwise we simply insert the new value to node's child map
     */
    for (auto &child : node->child_)
    {
      if (child.first.front() == input_key.front())
      {
        auto prev_key = child.first;
        std::string::iterator prev_mismatch, input_mismatch;

        std::tie(prev_mismatch, input_mismatch) =
            mismatch(prev_key.begin(), prev_key.end(), input_key.begin());
        assert(*prev_mismatch != *input_mismatch);

        std::string common_prefix{prev_key.begin(), prev_mismatch};

        node_ptr prev_node = child.second.release();
        auto nerased = node->child_.erase(prev_key);

        std::string prev_suffix{prev_mismatch, prev_key.end()};
        std::string input_suffix{input_mismatch, input_key.end()};

        if (input_mismatch == input_key.end()) /* branching node holds new data */
        {
          assert(input_key == common_prefix);
          assert(prev_key != common_prefix);

          node->child_.emplace(
              std::make_pair(common_prefix, newNode(node, value)));
          auto branch_node = node->child_.at(common_prefix).get();

          branch_node->child_.emplace(
              std::make_pair(prev_suffix, std::unique_ptr<TrieNode>(prev_node)));
          return node;
        }
        else /* branching node points to previous and new node */
        {
          assert(input_key != common_prefix);
          assert(prev_key != common_prefix);

          node->child_.emplace(
              std::make_pair(common_prefix, newNode(node)));
          auto branch_node = node->child_.at(common_prefix).get();

          branch_node->child_.emplace(
              std::make_pair(prev_suffix, std::unique_ptr<TrieNode>(prev_node)));
          branch_node->child_.emplace(
              std::make_pair(input_suffix, newNode(node, value)));
          return branch_node;
        }
      }
    }

    node->child_.emplace(
        std::make_pair(input_key, newNode(node, value)));

    return node;
  }

  auto newNode(node_ptr parent, const value_type &value) -> uniq_node_ptr
  {
    return std::make_unique<TrieNode>(parent, value.second);
  }
  auto newNode(node_ptr parent) -> uniq_node_ptr
  {
    return std::make_unique<TrieNode>(parent);
  }

public:
  uniq_node_ptr root_;
  size_t size_ = 0;

public:
  friend inline auto operator<<(std::ostream &strm, Trie &t) -> std::ostream &
  {
    return strm << *t.root_ << std::endl;
  }
  friend inline auto operator<<(std::ostream &strm, TrieNode &node) -> std::ostream &
  {
    strm << "|-" << node.data_ << std::endl;
    if (node.child_.size())
      strm << node.child_ << std::endl;
    return strm;
  }

  template <typename X, typename Y = uniq_node_ptr>
  friend inline auto operator<<(std::ostream &strm, const std::unordered_map<X, Y> &m) -> std::ostream &
  {
    for (auto &item : m)
      strm << "{ " << item.first << "\n\t" << *item.second.get() << " }" << std::endl;
    return strm;
  }
};
}
#endif