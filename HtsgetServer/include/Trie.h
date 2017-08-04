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
#include <initializer_list>

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
      : root_(std::make_unique<TrieNode>(nullptr)), size_(0){};

  /**
   * @brief   Given a key, find a node with identical prefix
   *          Return pointer to such node if exists, otherwise to root_
   */
  auto find(const std::string &key) -> node_ptr
  {
    node_ptr curr = root_.get();
    while (is_internal(curr))
    {
    }
  }

  /**
   * @brief   Inserts data to Trie given key 
   */
  auto insert(const value_type &value) -> node_ptr
  {
    node_ptr node;
    std::string input_key;
    std::tie(node, input_key) = find_to_insert(value.first);

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
      auto prev_key = child.first;
      if (prev_key.front() == input_key.front())
      {
        std::string::iterator prev_mismatch, input_mismatch;

        std::tie(prev_mismatch, input_mismatch) =
            mismatch(prev_key.begin(), prev_key.end(), input_key.begin());

        std::string common_prefix{prev_key.begin(), prev_mismatch};
        std::string prev_suffix{prev_mismatch, prev_key.end()};
        std::string input_suffix{input_mismatch, input_key.end()};

        node_ptr prev_node = child.second.release();
        node->child_.erase(prev_key);

        assert(prev_key != common_prefix);

        /**
         *  {common_prefix == input_suffix}\
         *                                |- prev_suffix
         */
        if (input_mismatch == input_key.end())
        {
          node->child_.emplace(
              std::make_pair(common_prefix, newNode(node, value)));
          auto branch_node = node->child_.at(common_prefix).get();

          branch_node->child_.emplace(
              std::make_pair(prev_suffix, std::unique_ptr<TrieNode>(prev_node)));
          return node;
        }
        /**
         *  common_prefix\
         *                |- input_suffix
         *                |- prev_suffix
         */
        else
        {
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
    /**
     *  {common_prefix == prev_suffix}\
     *                                |- input_suffix
     */
    node->child_.emplace(
        std::make_pair(input_key, newNode(node, value)));

    return node;
  }

  /** 
   * @brief   Given a key, find the node to insert to
   * 
   * invariant 
   *  suffix.substr(0, i) is not contained in node's map
   *  i.e. suffix must be inserted in node's map in some way
   */
  auto find_to_insert(const std::string &key) -> std::pair<node_ptr, std::string>
  {
    std::string dup_key{key};
    return std::make_pair(find_in(dup_key, root_.get()), dup_key);
  }

  auto newNode(node_ptr parent, const value_type &value) -> uniq_node_ptr
  {
    size_ += 1;
    return std::make_unique<TrieNode>(parent, value.second);
  }
  auto newNode(node_ptr parent) -> uniq_node_ptr
  {
    size_ += 1;
    return std::make_unique<TrieNode>(parent);
  }

public:
  uniq_node_ptr root_;
  size_t size_ = 0;

private:
  /**
   * @brief   Determine if a node is a leaf / internal node
   * A node is a leaf if does not have children, otherwise is an internal node
   */
  auto static is_leaf(node_ptr node) -> bool
  {
    return node->child_.size() == 0;
  }
  auto static is_internal(node_ptr node) -> bool
  {
    return node->child_.size() != 0;
  }

  /**
   * @brief   Find a TrieNode in Trie rooted at node 
   *          with key equivalent to key, If no exact match, 
   *          Return the last node with a common prefix
   */
  auto static find_in(std::string &key, node_ptr node) -> node_ptr
  {
    if (!key.size())
      return node;

    if (is_leaf(node))
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

public:
  /**
   * @brief   Finds the child of given node with corresponding prefix
   */
  friend inline auto get_child(const node_ptr &node, std::string prefix) -> node_ptr
  {
    return node->child_.at(prefix).get();
  }
  friend inline auto get_child(const uniq_node_ptr &node, std::string prefix) -> node_ptr
  {
    return node.get()->child_.at(prefix).get();
  }

  friend inline auto operator<<(std::ostream &strm, Trie &t) -> std::ostream &
  {
    strm << "size: " << t.size_ << std::endl;
    return strm << *t.root_ << std::endl;
  }
  friend inline auto operator<<(std::ostream &strm, TrieNode &node) -> std::ostream &
  {
    static std::string::size_type depth = 0;
    strm << "\t\t" << node.data_ << std::endl;
    if (node.child_.size())
    {
      depth += 1;
      for (auto &item : node.child_)
        strm << std::string{"  ", depth} << "|-" << item.first << "\\"
             << *item.second.get();
      depth -= 1;
    }
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