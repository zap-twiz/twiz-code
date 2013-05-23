#ifndef INCLUDED_FIXED_LOW_CHIP_H_
#define INCLUDED_FIXED_LOW_CHIP_H_

#include "chip/chip_description.h"

class FixedLowChip : public Chip {
 public:
  FixedLowChip(ChipDescription const & description) : Chip(description) {
   initialize();
  }
  virtual ~FixedLowChip() {}

  void evaluate();
 private:
  void initialize() {
    addOutputPins(1);
  }
};

void FixedLowChip::evaluate() {
  output_pins()[0].state() = OutputPin::kLow;
}

class FixedLowBuilder : public ChipBuilder {
 public:
  virtual ~FixedLowBuilder() {}

  virtual FixedLowChip* CreateInstance();
  virtual ChipDescription const * description() const {
    return &getDescription();
  }
 private:
  static ChipDescription const & getDescription();
};

ChipDescription const & FixedLowBuilder::getDescription() {
  static ChipDescription* k_description = NULL;
  if (!k_description) {
    std::string name = std::string("low");
    k_description = ChipDescription::Create(
      name,
      std::vector<std::string>(),
      CreateStrings("output", 1));
  }
  return *k_description;
}

FixedLowChip* FixedLowBuilder::CreateInstance() {
  FixedLowChip *instance = new FixedLowChip(*description());
  return instance;
}

#endif  // INCLUDED_FIXED_LOW_CHIP_H_
