#ifndef INCLUDED_LEXER_TRIE_H_
#define INCLUDED_LEXER_TRIE_H_

#include "base/trie_node.h"


template <typename T, typename V, typename C>
class TrieMap {
 public:
  typedef T KeyElement;
  typedef V Value;
  typedef C Container;  // The types that are stored in the trie map.

 private:
  template <typename T, typename C>
  struct ContainerTraits {
    typedef C Container;
    typedef T Type;
  };

  typedef TrieNode<KeyElement, Value> TrieNode;
  typedef ContainerTraits<KeyElement, Container> KeyTraits;

 public:
  class Iterator {
   public:
    Iterator() : current_node_(NULL) {}
    explicit Iterator(TrieNode* node) : current_node_(node) {}

    Iterator(Iterator const& iterator)
        : current_node_(iterator.current_node_) {}

    // prefix ++Iterator
    Iterator& operator++() {
      current_node_ = current_node_->NextExistingNode();
      return *this;
    }

    // postfix Iterator++
    Iterator operator++(int) {
      Iterator iter(*this);
      current_node_ = current_node_->NextExistingNode();
      return iter;
    }

    Iterator next(KeyElement const& value) {
      return Iterator(current_node_->Next(value));
    }

    bool exists() const {
      return current_node_ && current_node_->exists();
    }

    // Note that a reference is NOT returned
    Container const operator*() const;
    Container const operator->() const { return this->operator*(); }

    Value& value() {
      return *current_node_->value();
    }

    Value const& value() const {
      return *current_node_->value();
    }

    bool operator==(Iterator const& rhs) const {
      return rhs.current_node_ == current_node_;
    }

    bool operator!=(Iterator const& rhs) const {
      return !(rhs == *this);
    }

    Iterator& operator=(Iterator const& rhs) {
      current_node_ = rhs.current_node_;
      return *this;
    }

   private:
    TrieNode* current_node_;
    friend class TrieMap;  // The map needs to access the actual location of
                           // the iterator.  See TrieMap<...>::erase.
  };

  TrieMap();
  TrieMap(TrieMap<T, V, C> const& trie)
      : root_(trie.root_ ? trie.root_->Clone() : NULL),
        count_(trie.count()) {
  }
  ~TrieMap();

  TrieMap<T, V, C>& operator=(TrieMap<T, V, C> const& trie);

  void insert(Container const& key,
              Value const& value);
  void erase(Iterator& position);

  Iterator find(Container const& value);
  Iterator root() { return Iterator(root_); }

  Iterator begin() {
    if (root_ && root_->exists())
      return Iterator(root_);
    return Iterator(root_->NextExistingNode());
  }
  Iterator end() { return Iterator(NULL); }

  bool empty() const { return 0 == count_; }

  void clear();

  size_t count() const { return count_; }

  // If no match was found, |matched_length| is assigned -1.
  Iterator find(Container const& value, size_t* matched_length);

 private:
  TrieNode* GetRoot();

  // Find the root of the common prefix.  Given |value|, return a node,
  // |node| pointing into the trie that is at |matched_length| depth.
  TrieNode* FindCommonPrefix(Container const& value, size_t* matched_length);

  // The number of elements in the trie.
  size_t count_;

  // The root always exists, even for empty tries.
  TrieNode* root_;
};

#include "base/trie_inl.h"

#endif  // INCLUDED_LEXER_TRIE_H_
