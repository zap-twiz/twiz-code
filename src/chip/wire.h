#ifndef INCLUDED_WIRE_H_
#define INCLUDED_WIRE_H_

#include "pin.h"
#include "named_element.h"

class Wire : public NamedElement {
 public:
  Wire(std::string const & name) : NamedElement(name) {}
  virtual ~Wire() { destroy(); }

  void setSource(OutputPin* pin) {
    if (pin) {
      pin->addWire(this);
    }
    source_ = pin;
  }
  void addConnection(InputPin* pin) {
    connections_.push_back(pin);
    pin->setWire(this);
  }

  std::vector<InputPin*> const & connections() const { return connections_; }
  std::vector<InputPin*>& connections() { return connections_; }

  OutputPin* source() { return source_; }
  OutputPin const * source() const { return source_; }

 private:
  void destroy() {
    if (source_) {
      source_->removeWire(this);
    }

    std::vector<InputPin*>::iterator iter(connections_.begin()),
        end(connections_.end());
    for (; iter != end; ++iter) {
      (*iter)->setWire(NULL);
    }
  }

  std::vector<InputPin*> connections_;
  OutputPin* source_;
};

#endif  // INCLUDED_WIRE_H_
