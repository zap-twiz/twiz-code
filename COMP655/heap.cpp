// heap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <functional>

template < class T, class K, class A = std::allocator<T>>
class Heap {
 public:
  typedef T Type;
  typedef K Comparator;
  typedef A Allocator;

  Heap() {}
  ~Heap() {}

  Type const & top() const {
    return buffer_.front();
  }
  Type& top() {
    return buffer_.front();
  }

  Type RemoveTop();

  bool empty() const {
    return buffer_.empty();
  }

  void clear() {
    buffer_.clear();
  }

  void insert(Type const& value);

 private:
  static int parent(int index) { return index / 2; }
  static int right_child(int index) { return index * 2 + 1; }
  static int left_child(int index) { return index * 2; }
 
  typedef std::vector<Type, A> Buffer;
  Buffer buffer_;
  Comparator less_;
};

template < class T, class K, class A >
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

template < class T, class K, class A >
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
        less_(buffer_[right_index - 1], buffer_[min_index - 1]))
      min_index = right_index;
    }

    if (left_index < last_index &&
        less_(buffer_[left_index - 1], buffer_[min_index - 1]))
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

#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
  Heap<int, std::less<int>> heap;

  for (int x = 0; x < 10000; ++x ) {
    heap.insert(10 - x);
  }

  while (!heap.empty()) {
    cout << heap.top() << endl;
    heap.RemoveTop();
  }

  return 0;
}

