
template <typename T>
StreamWithObserver<T>::StreamWithObserver(BaseStream& base_stream)
    : NestedStream<T>(base_stream),
      observer_(NULL) {
}

template <typename T>
StreamWithObserver<T>::Type StreamWithObserver<T>::Get() {
  Type value = NestedStream<T>::Get();
  if (observer_)
    observer_->observe(value);
  return value;
}

