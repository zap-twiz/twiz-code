#ifndef INCLUDED_STERAMS_COUNTER_H_
#define INCLUDED_STERAMS_COUNTER_H_

#include "streams/stream_with_observer.h"

class Counter {
 public:
  virtual ~Counter() {}
  virtual size_t GetCount() const = 0;
};

template <typename T>
class CountObserver : public Counter,
                      public ObservedStream<T>::Observer {
 public:
  CountObserver(const T& line) : count_(0), line_(line) {}

  virtual bool Observe(T value) {
    if (line_ == value)
      ++count_;

    return true;
  }

  virtual void Unobserve(T value) {
    if (line_ == value)
      --count_;
  }

  virtual size_t GetCount() const { return count_; }
  void SetCount(size_t count) { count_ = count; }

 private:
  size_t count_;
  const T line_;
};


#endif  // INCLUDED_STERAMS_COUNTER_H_