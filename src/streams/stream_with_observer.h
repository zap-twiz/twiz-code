#ifndef INCLUDED_STREAMS_STREAM_WITH_OBSERVER_H_
#define INCLUDED_STREAMS_STREAM_WITH_OBSERVER_H_

#include "streams/buffered_stream.h"

template <typename T>
class ObservedStream : public BufferedStream<T> {
 public:
  class Observer {
   public:
    virtual ~Observer() {}
    virtual bool Observe(Type value) = 0;
    virtual void Unobserve(Type value) = 0;
  };

  ObservedStream(BaseStream& base_stream);
  virtual Type Get();

  virtual void Unget(const Type& value);

  Observer* observer() { return observer_; }
  void set_observer(Observer* observer) { observer_ = observer; }

 private:
  Observer* observer_;
};

#include "stream_with_observer_inl.h"
#endif  // INCLUDED_STREAMS_STREAM_WITH_OBSERVER_H_
