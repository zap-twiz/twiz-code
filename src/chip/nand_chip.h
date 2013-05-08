#ifndef INCLUDED_NAND_CHIP_H_
#define INCLUDED_NAND_CHIP_H_

#include "chip/chip.h"
#include "chip/chip_description.h"

#include <string>
#include <sstream>

template <int WAY>
class NandChip : public Chip {
 public:
  NandChip(ChipDescription const * description) : Chip(description) {
    initialize();
  }
  virtual ~NandChip() {}

  void evaluate();

 private:
  void initialize() {
    addInputPins(WAY);
    addOutputPins(1);
  }

  NandChip(NandChip const &);
  NandChip<WAY> operator=(NandChip<WAY> const &);
};

template <int WAY>
void NandChip<WAY>::evaluate() {
  std::vector<InputPin>& inputs = input_pins();
  std::vector<OutputPin>& outputs = output_pins();

  bool state = IsHigh(inputs[0].wire()->source()->state());
  for (unsigned int x = 1; x < inputs.size(); ++x) {
    state = state && IsHigh(inputs[x].wire()->source()->state());
  }

  outputs[0].state() = !state ? OutputPin::kHigh : OutputPin::kLow;
}

template <int WAY>
class NandBuilder : public ChipBuilder {
 public:
  virtual ~NandBuilder() {}

  virtual NandChip<WAY>* CreateInstance();
  virtual ChipDescription const * description() const {
    return GetDescription();
  }
 private:
  static ChipDescription* GetDescription();
};

template <int WAY>
ChipDescription* NandBuilder<WAY>::GetDescription() {
  static ChipDescription* k_description = NULL;
  if (!k_description) {
    std::ostringstream s;
    s << std::string("Nand") << WAY;
    std::string name = s.str();
    k_description = ChipDescription::Create(
      name,
      CreateStrings("input", WAY),
      CreateStrings("output", 1));
  }
  return k_description;
}

template <int WAY>
NandChip<WAY>* NandBuilder<WAY>::CreateInstance() {
  NandChip<WAY> *instance = new NandChip<WAY>(description());
  return instance;
}

#endif  // INCLUDED_NAND_CHIP_H_
