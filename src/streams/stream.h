#ifndef INCLUDED_STREAMS_STREAMS_H_
#define INCLUDED_STREAMS_STREAMS_H_


template <typename T>
class Stream {
 public:
  typedef T Type;
  virtual ~Stream() {}

  virtual T Get() = 0;
  virtual size_t GetN(T* data, size_t count) = 0;
  virtual bool IsEOS() const = 0;
};

#endif  // INCLUDED_STREAMS_STREAMS_H_
