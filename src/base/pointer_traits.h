#ifndef INCLUDED_BASE_POINTER_TRAITS_H_
#define INCLUDED_BASE_POINTER_TRAITS_H_

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

template <class T>
class Pointer;

template <class T>
class Value;

template <class T>
class IsPrimitive {
 public:
  static const bool kValue = false;

  typedef Pointer<T> PointerSemantic;
};

#define DECLARE_PRIMITIVE_SPECIALIZATION(type) \
template <> \
class IsPrimitive<type> { \
 public: \
  static const bool kValue = true; \
  typedef Value<type> PointerSemantic; \
};

DECLARE_PRIMITIVE_SPECIALIZATION(bool)
DECLARE_PRIMITIVE_SPECIALIZATION(char)
DECLARE_PRIMITIVE_SPECIALIZATION(short)
DECLARE_PRIMITIVE_SPECIALIZATION(int)
DECLARE_PRIMITIVE_SPECIALIZATION(float)
DECLARE_PRIMITIVE_SPECIALIZATION(double)

template <class P>
class IsPrimitive<P*> {
 public:
  static const bool kValue = true;
  typedef Value<P*> PointerSemantic;
};

template <class T>
class Pointer {
 public:
  Pointer() : value_(NULL) {}
  ~Pointer() { clear_value(); }

  bool IsNull() const { return NULL == value_; }

  T& value() { return *value_; }
  T const & value() const { return *value_; }

  void set_value(T const& value) {
    if (IsNull()) {
      value_ = new T(value);
    } else {
      value_->~T();
      value_ = new(value_) T(value);
    }
  }

  void clear_value() {
    delete value_;
    value_ = NULL;
  }

 private:
  T* value_;
};

template <class T>
class Value {
 public:
  Value() : exists_(false) {}
  ~Value() { clear_value(); }

  bool IsNull() const { return false == exists_; }

  T& value() { return value_; }
  T const & value() const { return value_; }

  void set_value(T const& value) {
    exists_ = true;
    value_ = value;
  }

  void clear_value() {
    exists_ = false;
  }

 private:
  bool exists_;
  T value_;
};

template <class T>
class ScopedPointerSemantic : public IsPrimitive<T>::PointerSemantic {
};

#endif  // INCLUDED_BASE_POINTER_TRAITS_H_
