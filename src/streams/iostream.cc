#include "streams/iostream.h"


IoStream::Type IoStream::Get() {
  char temp = next_value_;
  next_value_ = stream_ ? stream_->get() : NULL;
  return temp;
}

size_t IoStream::GetN(Type* data, size_t count) {
  return 0;
}

bool IoStream::IsEOS() const {
  return !stream_ || !stream_->good();
}
