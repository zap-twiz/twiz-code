#ifndef INCLUDED_HEAP_H__
#define INCLUDED_HEAP_H__

#include <vector>

template <class T, class K, class A = std::allocator<T>>
class Heap {
 public:
  typedef T Type;
  typedef K Comparator;
  typedef A Allocator;

  Heap() {}
  ~Heap() {}

  Type const & top() const { return buffer_.front(); }
  Type& top() { return buffer_.front(); }

  Type RemoveTop();

  bool empty() const { return buffer_.empty(); }

  void clear() { buffer_.clear(); }

  size_t size() const { return buffer_.size(); }

  void insert(Type const& value);

 private:
  static int parent(int index) { return index / 2; }
  static int right_child(int index) { return index * 2 + 1; }
  static int left_child(int index) { return index * 2; }
 
  typedef std::vector<Type, A> Buffer;
  Buffer buffer_;
  Comparator less_;
};

#include "heap_inl.h"

#endif  // INCLUDED_HEAP_H__
