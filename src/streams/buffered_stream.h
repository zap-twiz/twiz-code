#ifndef INCLUDED_STREAMS_BUFFERED_STREAM_H_
#define INCLUDED_STREAMS_BUFFERED_STREAM_H_

#include "streams/nested_stream.h"

#include <vector>

template <typename T>
class BufferedStream : public NestedStream<T> {
 public:
  BufferedStream(BaseStream& input_stream) : NestedStream<T>(input_stream) {}

  virtual Type Get() {
    if (peek_buffer_.empty()) {
      return base_stream_.Get();
    } else {
      Type value(*peek_buffer_.rbegin());
      peek_buffer_.pop_back();
      return value;
    }
  }

  virtual void Unget(const Type& value) {
    peek_buffer_.push_back(value);
  }

  virtual size_t GetN(Type* data, size_t count) {
    // Not implemented
    return 0;
  }

  virtual bool IsEOS() const {
    return peek_buffer_.empty() && base_stream_.IsEOS();
  }

 private:
  std::vector<T> peek_buffer_;
};

#endif  // INCLUDED_STREAMS_BUFFERED_STREAM_H_
