#ifndef INCLUDED_NAMED_ELEMENT_H_
#define INCLUDED_NAMED_ELEMENT_H_

#include <string>

class NamedElement {
 public:
  NamedElement() {}
  NamedElement(std::string const & name) : name_(name) {}
  virtual ~NamedElement() {}

  std::string const & name() const { return name_; }
  void set_name(std::string const & name) { name_ = name; }

 private:
  std::string name_;
};

#endif // INCLUDED_NAMED_ELEMENT_H_
