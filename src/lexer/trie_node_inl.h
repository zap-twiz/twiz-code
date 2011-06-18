

template <class T, class V>
TrieNode<T, V>* TrieNode<T, V>::NextNode() {
  if (children_.empty()) {
    TrieNode* current_node = this;
    TrieNode* next_node = current_node->parent_;
    while(next_node) {
      // Check if we went up via the right-most child.  If so, then continue
      // up through the parent chain.
      ChildrenMap::iterator iter =
          next_node->FindChildWithNode(current_node);
      ++iter;
      if (next_node->children_.end() == iter) {
        current_node = next_node;

        // If we go up via the root, then the parent will be NULL.
        next_node = next_node->parent_;
      } else {
        next_node = iter->second;
        break;
      }
    }
    return next_node;
  } else {
    return children_.begin()->second;
  }
}

template <class T, class V>
TrieNode<T, V>* TrieNode<T, V>::NextExistingNode() {
  TrieNode* current_node = this;
  do {
    current_node = current_node->NextNode();
  } while (current_node && !current_node->exists());
  return current_node;
}

template <class T, class V>
TrieNode<T, V>* TrieNode<T, V>::Next(KeyElement const& value) {
  ChildrenMap::iterator find(children_.find(value));
  if (children_.end() == find)
    return NULL;

  return find->second;
}

template <class T, class V>
TrieNode<T, V>* TrieNode<T, V>::Clone() const {
  TrieNode* new_node = new TrieNode(NULL);

  if (!value_.IsNull())
    new_node->value_.set_value(value_.value());

  ChildrenMap::const_iterator iter(children_.begin()), end(children_.end());
  for (; iter != end; ++iter) {
    TrieNode* new_child = iter->second->Clone();
    new_child->parent_ = new_node;
    new_node->children_[iter->first] = new_child;
  }
  return new_node;
}

template <class T, class V>
void DeleteTrieDescendents(TrieNode<T, V>* root) {
  typename TrieNode<T, V>::ChildrenMap::iterator
      iter(root->children()->begin()), end(root->children()->end());
  for (; iter != end; ++iter)
    DeleteTrieDescendents(iter->second);

  delete root;
}