#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <utility>
#include <unordered_map>
#include <list>
#include <vector>
#include <cstddef>
#include <iterator>

namespace Http
{

template <typename T>
class Trie
{
  public:
    class TrieNode;
    class TrieIterator;
    class TrieConstIterator;

    using key_type = std::string;
    using mapped_type = T;
    using value_type = std::pair<const key_type, T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    // using iterator = TrieIterator;
    // using const_iterator = TrieIterator;
    using iterator = TrieNode *;
    using const_iterator = const TrieNode *;

  public:
    // class TrieIterator : public std::iterator<
    //                          std::bidirectional_iterator_tag, // Category
    //                          value_type                       // value_type
    //                          >
    // {
    //   public:
    //     explicit TrieIterator()
    //         : iterator_(nullptr){};

    //     TrieIterator &operator++() // pre-increment
    //     {
    //         assert(iterator_ != nullptr);
    //         return *iterator_;
    //     }
    //     TrieIteratro &operator++(int) // post-increment
    //     {
    //         assert(iterator_ != nullptr);
    //     }
    //     bool operator==(const TrieIterator &rhs) const
    //     {
    //         return iterator_ == rhs.iterator_;
    //     }
    //     bool operator!=(const TrieIterator &rhs) const
    //     {
    //         return !(iterator_ == rhs.iterator_);
    //     }
    //     reference operator*() const { return iterator_->data_; }

    //   private:
    //     TrieNode *iterator_;
    // };

    /**
     * @brief   Node of a Trie
     * 
     * @param   prefix_     prefix string of edge from parent_
     * @param   data_       auxiliary data 
     *                      internal node:  may or may not contain data_
     *                      leaf node: must has valid data_
     * @param   parent_     pointer to parent node, nullptr for root_
     * @param   child_      pointer to first child
     * @param   next_       pointer to the next sibling
     */
    class TrieNode
    {
      public:
        using node_ptr = TrieNode *;

      public:
        std::string prefix_;
        mapped_type data_;

        TrieNode *parent_;
        TrieNode *child_;
        TrieNode *next_;

        TrieNode(std::string prefix, const T &data,
                 TrieNode *parent = nullptr, TrieNode *child = nullptr, TrieNode *next = nullptr)
            : prefix_(prefix), data_(data), parent_(parent), child_(child), next_(next){};

        bool operator==(const TrieNode &rhs) const
        {
            return (data_ == rhs.data_) &&
                   (prefix_ == rhs.prefix_) &&
                   (parent_ == rhs.parent_) &&
                   (child_ == rhs.child_);
        }
        bool operator!=(const TrieNode &rhs) const
        {
            return !(this == data_);
        }
        reference operator*() const
        {
            return data_;
        }
        /**
         * @brief   Find last node in the linked list of nodes
         */
        static TrieNode &last(TrieNode &node)
        {
            while (node.next_)
                node = node.next_;
            return node;
        }
        /**
         * @brief   Find minimum/maximum of TrieNode in Trie
         */
        static TrieNode &minimum(TrieNode &node)
        {
            if (node.data_)
                return node;

            assert(node.child_ != nullptr || node.next_ != nullptr);
            if (node.child_)
                return minimum(node.child_);
            else
                return minimum(node.next_);
        }
        static TrieNode &maximum(TrieNode &node)
        {
            node = last(node);
            while (node.child_)
                node = last(node.child_);
            return node;
        }
        /**
         * @brief   Finds successor of TrieNode
         * 
         * If TrieNode has child 
         *  -- the minimum of Trie rooted at child_ is the successor
         * Otherwise TrieNode has a larger sibling 
         *  -- the minimum of Trie rooted at next_ is the successor
         * Otherwise, Node is at end of list with no child
         *  -- find successor in node's parent, if applicable
         */
        static TrieNode &successor(TrieNode &node)
        {
            if (node.child_)
                return minimum(node.child_);
            if (node.next_)
                return minimum(node.next_);

            TrieNode &curr = node.parent_;
            while (curr != nullptr)
            {
                if (curr.next_)
                    return minimum(curr.next_);
                node = curr;
                curr = curr.parent_;
            }
            return nullptr;
        }
        TrieNode &operator++()
        {
            return successor(this);
        }
        TrieNode &operator--()
        {
            return predecessor();
        }
    };

    Trie()
        : root_(nullptr), size_(0){};

    /**
     * @brief   Find a TrieNode with key equivalent to key
     *          If no exact match, 
     */
    auto find(const value_type &value) -> iterator;
    auto find(const value_type &value) const -> const_iterator;

    /**
     * @brief   Iterator operations
     */
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    /**
     * @brief   Inserts data to Trie given key 
     */
    auto insert(const value_type &value) -> std::pair<iterator, bool>;

  private:
    TrieNode root_;
    size_type size_;
};
}
#endif