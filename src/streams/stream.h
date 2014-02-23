#ifndef INCLUDED_STREAMS_STREAMS_H_
#define INCLUDED_STREAMS_STREAMS_H_

#include "base/base.h"


template <typename T>
class Stream {
 public:
  typedef T Type;
  Stream() {}
  virtual ~Stream() {}

  virtual T Get() = 0;
  virtual size_t GetN(T* data, size_t count) = 0;
  virtual bool IsEOS() const = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Stream);
};

#endif  // INCLUDED_STREAMS_STREAMS_H_
