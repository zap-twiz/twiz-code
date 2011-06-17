#ifndef INCLUDED_STREAMS_IOSTREAM_H_
#define INCLUDED_STREAMS_IOSTREAM_H_

#include "streams/stream.h"

#include <istream>


class IoStream : public Stream<char> {
 public:
  IoStream() : stream_(NULL) {}
  IoStream(std::istream* stream) : stream_(NULL) {
    set_stream(stream);
  }

  void set_stream(std::istream* stream) {
    stream_ = stream;
    next_value_ = stream_ ? stream_->get() : NULL;
  }

  // Stream<char> overrides
  virtual Type Get();
  virtual size_t GetN(Type* data, size_t count);
  virtual bool IsEOS() const;

 private:
   std::istream* stream_;
   char next_value_;
};

#endif  // INCLUDED_STREAMS_IOSTREAM_H_
