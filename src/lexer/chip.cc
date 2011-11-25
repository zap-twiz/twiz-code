
#include <vector>

class Chip;
class Wire;

class Pin {
 public:
  Pin(Chip* chip) : wire_(NULL), chip_(chip) {}
  Pin() : wire_(NULL), chip_(NULL) {}

  Wire* wire() { return wire_; }
  Wire const* wire() const { return wire_; }

  Chip* chip() { return chip_; }
  Chip const* chip() const { return chip_; }

  void set_wire(Wire* wire) { wire_ = wire; }
  void set_chip(Chip* chip) { chip_ = chip; }
 private:
  Wire* wire_;
  Chip* chip_;
};

class Wire {
 public:
  typedef std::vector<Pin*> PinSet;
  typedef enum State {
    UNKNOWN,
    HIGH,
    LOW
  };

  Wire(State value) : value_(value) {}
  Wire() : value_(UNKNOWN) {}

  State& value() { return value_; }
  State const value() const { return value_; }

  PinSet& pins() { return pins_; }
  PinSet const& pins() const { return pins_; }

 private:
  PinSet pins_;
  State value_;
};

class ChipTraits {
 public:
};

class Chip {
 public:
  Chip(int num_inputs, int num_outputs)
      : input_pins_(num_inputs), output_pins_(num_outputs) {
    PinSet::iterator iter(input_pins_.begin()), end(input_pins_.end());
    for (; iter != end; ++iter)
      iter->set_chip(this);

    iter = output_pins_.begin();
    end = output_pins_.end();
    for (; iter != end; ++iter)
      iter->set_chip(this);
  }

  Pin& output_pin(int index) { return output_pins_[index]; }
  Pin const& output_pin(int index) const { return output_pins_[index]; }

  Pin& input_pin(int index) { return input_pins_[index]; }
  Pin const& input_pin(int index) const { return input_pins_[index]; }

  virtual void ApplyInputs() = 0;
 private:
  typedef std::vector<Pin> PinSet;
  PinSet input_pins_;
  PinSet output_pins_;
};

template <int way>
class NandChip : public Chip {
 public:
  NandChip() : Chip(way, 1) {}

  virtual void ApplyInputs();
};

template <int way>
void NandChip<way>::ApplyInputs() {
  bool result = true;
  for (int x = 0; x < way; ++x) {
    result = result && (input_pin(x).wire()->value() == Wire::HIGH);
  }

  output_pin(0).wire()->value() = result ? Wire::LOW : Wire::HIGH;
}