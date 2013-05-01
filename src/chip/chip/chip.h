#ifndef INCLUDED_CHIP_H_
#define INCLUDED_CHIP_H_

#include "named_element.h"
#include "pin.h"

#include <vector>

class Chip;
class ChipDescription;
class ChipBuilder {
 public:
  virtual Chip* CreateInstance() = 0;
  virtual ChipDescription const * description() const = 0;
 private:
};

class Chip : public NamedElement {
 public:
  Chip(ChipDescription const * description) : description_(description) {};
  virtual ~Chip() {};

  std::vector<InputPin>& input_pins() { return input_pins_; }
  std::vector<OutputPin>& output_pins() { return output_pins_; }

 protected:
  void addInputPins(int count) {
    input_pins_.resize(count);
  }

  void addOutputPins(int count) {
    output_pins_.resize(count);
  }

 private:
  std::vector<InputPin> input_pins_;
  std::vector<OutputPin> output_pins_;

  ChipDescription const * description_;
};


#endif  // INCLUDED_CHIP_H_
