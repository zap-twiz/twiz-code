#ifndef INCLUDED_BASE_TRIE_NODE_H_
#define INCLUDED_BASE_TRIE_NODE_H_

#include "base/pointer_traits.h"


// compile with: /EHsc
#define _DEFINE_DEPRECATED_HASH_CLASSES 0
#include <hash_map>
#undef _DEFINE_DEPRECATED_HASH_CLASSES

template <class T, class V>
class TrieNode {
 public:
  typedef T KeyElement;
  typedef V Value;
  typedef stdext::hash_map<KeyElement, TrieNode*> ChildrenMap;

  explicit TrieNode(TrieNode* parent)
      : parent_(parent) {}
  ~TrieNode() { ClearValue(); }

  ChildrenMap* children() { return &children_; }
  ChildrenMap const * children() const { return &children_; }

  // Next node, according to an in-order walk of the trie.
  TrieNode<KeyElement, Value>* NextNode();
  TrieNode<KeyElement, Value>* NextExistingNode();

  // Returns the child node corresponding to |value|.
  TrieNode<KeyElement, Value>* Next(KeyElement const& value);

  // Duplicates the node tree
  TrieNode<KeyElement, Value>* Clone() const ;

  void AddChild(KeyElement const& value, TrieNode* node) {
    children_[value] = node;
    node->set_parent(this);
  }

  // Does not delete node!
  void RemoveChild(TrieNode* node) {
    ChildrenMap::iterator location(FindChildWithNode(node));
    children_.erase(location);
    node->set_parent(NULL);
  }

  bool exists() const { return !value_.IsNull(); }

  Value * value() {
    return !value_.IsNull() ? &value_.value() : NULL;
  }
  Value const* value() const {
    return !value_.IsNull() ? &value_.value() : NULL;
  }
  void set_value(Value const& value) { value_.set_value(value); }

  void ClearValue() { value_.clear_value(); }

  TrieNode* parent() { return parent_; }

  // Given a node, |node|, returns the iterator within the child set
  // that refers to that node as a child.
  typename ChildrenMap::iterator FindChildWithNode(TrieNode const * node) {
    typename ChildrenMap::iterator iter(children_.begin()),
        end(children_.end());
    for (; iter != end; ++iter) {
      if (iter->second == node)
        break;
    }
    return iter;
  }

 private:
  void set_parent(TrieNode* parent) { parent_ = parent; }

  ScopedPointerSemantic<Value> value_;
  TrieNode* parent_;
  ChildrenMap children_;

  // Disallow value semantics.
  TrieNode(TrieNode const & node) {}
};

// Delete |root| and all of its descendents.
template <class T, class V>
void DeleteTrieDescendents(TrieNode<T, V>* root);

#include "base/trie_node_inl.h"

#endif  // INCLUDED_BASE_TRIE_NODE_H_
