
template <typename T>
ObservedStream<T>::ObservedStream(BaseStream& base_stream)
    : BufferedStream<T>(base_stream),
      observer_(NULL) {
}

template <typename T>
typename ObservedStream<T>::Type ObservedStream<T>::Get() {
  Type value = BufferedStream<T>::Get();
  if (observer_) {
    // Read from the stream until the observer indicates that the value should
    // be returned.
    while (!observer_->Observe(value))
      value = BufferedStream<T>::Get();
  }
  return value;
}

template <typename T>
void ObservedStream<T>::Unget(const Type& value) {
  if (observer_)
    observer_->Unobserve(value);
  BufferedStream<T>::Unget(value);
}
