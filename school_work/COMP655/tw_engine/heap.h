#ifndef INCLUDED_HEAP_H__
#define INCLUDED_HEAP_H__

#include <vector>

// Template container class implementing a canonical, array-based heap.
template <class T, class K, class A = std::allocator<T>>
class Heap {
 public:
  typedef T Type;
  typedef K Comparator;
  typedef A Allocator;

  Heap() {}
  ~Heap() {}

  // Returns the top of the heap.
  Type const & top() const { return buffer_.front(); }
  Type& top() { return buffer_.front(); }

  // Pops the top of the heap, and returns the value.
  Type RemoveTop();

  bool empty() const { return buffer_.empty(); }

  // Empties the contents of the heap.
  void clear() { buffer_.clear(); }

  // Returns the number of elements in the heap.
  size_t size() const { return buffer_.size(); }

  // Insert |value| into the heap.
  void insert(Type const& value);

 private:
  // Helper routines for indexing during the heap trickle-up/trickle-down
  // algorithms.
  static int parent(int index) { return index / 2; }
  static int right_child(int index) { return index * 2 + 1; }
  static int left_child(int index) { return index * 2; }
 
  typedef std::vector<Type, A> Buffer;
  Buffer buffer_;
  Comparator less_;
};

#include "heap_inl.h"

#endif  // INCLUDED_HEAP_H__
