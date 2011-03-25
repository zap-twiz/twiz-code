
template <class T, class K, class A >
void Heap<T, K, A>::insert(Type const& value) {
  int index = buffer_.size() + 1;
  buffer_.push_back(value);

  // Bubble-up the newly inserted item.
  int parent_index = parent(index);
  while (0 != parent_index &&
         !less_(buffer_[parent_index - 1], value)) {
    std::swap(buffer_[index - 1], buffer_[parent_index - 1]);
    index = parent_index;
    parent_index = parent(parent_index);
  }
}

template <class T, class K, class A >
typename Heap<T, K, A>::Type Heap<T, K, A>::RemoveTop() {
  Type top_element = top();

  std::swap(buffer_.back(), buffer_.front());
  buffer_.pop_back();

  // Trickle down
  int index = 1;
  int const last_index = buffer_.size() + 1;
  for (;;) {
    int const right_index = right_child(index);
    int const left_index = left_child(index);

    int min_index = index;
    if (right_index < last_index &&
        less_(buffer_[right_index - 1], buffer_[min_index - 1])) {
      min_index = right_index;
    }

    if (left_index < last_index &&
        less_(buffer_[left_index - 1], buffer_[min_index - 1])) {
      min_index = left_index;
    }

    if (min_index != index) {
      std::swap(buffer_[min_index - 1], buffer_[index - 1]);
      index = min_index;
    } else {
      break;
    }
  }

  return top_element;
}
