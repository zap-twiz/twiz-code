

#include <string>

// Specialization for strings.
template <typename T, typename V, typename C>
template <typename Traits, typename Allocator>
struct TrieMap<T, V, C>::ContainerTraits<T, std::basic_string<T, Traits,
                                                              Allocator> > {
  typedef typename std::basic_string<T, Traits, Allocator> Container;
  typedef T Type;

  static inline Container& push_front(Container* container,
                                      Type const& value) {
    Container temp;
    temp.push_back(value);
    *container = temp + *container;
    return *container;
  }

  static inline size_t size(Container const& container) {
    return container.length();
  }
};


template <class T, class V, class C>
TrieMap<T, V, C>::TrieMap() : count_(0), root_(NULL) {
}

template <class T, class V, class C>
TrieMap<T, V, C>::~TrieMap() {
  clear();
};

template <class T, class V, class C>
void TrieMap<T, V, C>::insert(typename TrieMap<T, V, C>::Container const& key,
                              typename TrieMap<T, V, C>::Value const& value) {
  TrieNode* current_node = NULL;
  size_t offset = 0;
  current_node = FindCommonPrefix(key, &offset);

  size_t length = KeyTraits::size(key);
  for (; offset < length; ++offset) {
    TrieNode* next_node = new TrieNode(current_node);
    current_node->AddChild(key[offset], next_node);
    current_node = next_node;
  }

  // We only added to the container if the node didn't already exist
  if (!current_node->exists())
    ++count_;

  current_node->set_value(value);
}

template <class T, class V, class C>
typename TrieMap<T, V, C>::Container const
TrieMap<T, V, C>::Iterator::operator*() const {
  Container value;
  TrieNode* current_node = current_node_;

  // Walk up the trie, building the output until we reach the root.
  while (current_node->parent()) {
    KeyTraits::push_front(
        &value,
        current_node->parent()->FindChildWithNode(current_node)->first);
    current_node = current_node->parent();
  }
  return value;
}

template <class T, class V, class C>
TrieMap<T, V, C>& TrieMap<T, V, C>::operator=(TrieMap const& trie) {
  if (this == &trie)
    return *this;

  clear();
  root_ = trie.root_? trie.root_->Clone() : NULL;
  count_ = trie.count();

  return *this;
}

template <class T, class V, class C>
void TrieMap<T, V, C>::erase(typename TrieMap<T, V, C>::Iterator& location) {
  TrieNode* current_node = location.current_node_;
  current_node->ClearValue();
  if (location.current_node_->children()->empty()) {
    // If the node has no children, remove its entry from it's parent's set
    // of children.
    if (current_node->parent()) {
      current_node->parent()->RemoveChild(current_node);
      delete current_node;
    }
  }
  --count_;
}

template <class T, class V, class C>
typename TrieMap<T, V, C>::Iterator TrieMap<T, V, C>::find(
    typename TrieMap<T, V, C>::Container const & value) {
  TrieNode* current_node = NULL;
  size_t offset = 0;
  current_node = FindCommonPrefix(value, &offset);
  assert(current_node != NULL);

  if (offset == KeyTraits::size(value) && current_node->exists()) {
    return Iterator(current_node);
  }

  return end();
}

template <class T, class V, class C>
void TrieMap<T, V, C>::clear() {
  // Bail early if the trie is already empty
  if (!root_)
    return;

  DeleteTrieDescendents(root_);

  root_ = NULL;
  count_ = 0;
}

template <class T, class V, class C>
typename TrieMap<T, V, C>::Iterator TrieMap<T, V, C>::find(
    Container const& value, size_t* matched_length) {
  TrieNode* current_node = GetRoot();

  TrieNode* matched_node = NULL;
  size_t matched_offset = -1;

  size_t length = KeyTraits::size(value);
  for (size_t offset = 0; offset < length; ++offset){
    if (current_node->exists()) {
      matched_node = current_node;
      matched_offset = offset;
    }

    TrieNode* next_node = current_node->Next(value[offset]);
    if (!next_node)
      break;

    current_node = next_node;
  }

  if (matched_length)
    *matched_length = matched_offset;

  return Iterator(matched_node);
}

template <class T, class V, class C>
typename TrieMap<T, V, C>::TrieNode* TrieMap<T, V, C>::FindCommonPrefix(
    Container const& value, size_t* matched_length) {
  TrieNode* current_node = GetRoot();
  size_t offset = 0;
  for (; offset < KeyTraits::size(value); ++offset){
    TrieNode* next_node = current_node->Next(value[offset]);
    if (!next_node)
      break;
    current_node = next_node;
  }

  if (matched_length)
    *matched_length = offset;

  return current_node;
}

template <class T, class V, class C>
typename TrieMap<T, V, C>::TrieNode* TrieMap<T, V, C>::GetRoot() {
  if (!root_)
    root_ = new TrieNode(NULL);

  return root_;
}
