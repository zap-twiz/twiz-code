#ifndef INCLUDED_NAMED_ENTITY_H_
#define INCLUDED_NAMED_ENTITY_H_

#include <string>

class NamedEntity {
 public:
  explicit NamedEntity(int id) : id_(id) {}
  NamedEntity() : id_(0) {}
  virtual ~NamedEntity() {}

  int id() const { return id_; }
  void set_id(int id) { id_ = id; }

  std::string const & name() const { return name_; }
  void set_name(std::string const & name) { name_ = name; }
 private:
  std::string name_;
  int id_;
};

#endif  // INCLUDED_NAMED_ENTITY_H_
