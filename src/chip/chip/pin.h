#ifndef INCLUDED_PIN_H_
#define INCLUDED_PIN_H_

#include <vector>
#include <algorithm>

class Wire;
class OutputPin {
 public:
  enum PinState {
    kLow = 0,
    kHigh = 1,
    kFalse = 0,
    kTrue = 1,
    kUnknown = 2
  };

  OutputPin() : state_(kUnknown) {};

  PinState& state() { return state_; }
  PinState const & state() const { return state_; }

  void addWire(Wire* wire) { wires_.push_back(wire); }
  void removeWire(Wire* wire) {
    wires_.erase(std::find(wires_.begin(), wires_.end(), wire));
  }

  std::vector<Wire*> const & wires() const { return wires_; }
  std::vector<Wire*> & wires() { return wires_; }

 private:
  std::vector<Wire*> wires_;
  PinState state_;
};

bool IsHigh(OutputPin::PinState const state) {
  return OutputPin::kHigh == state ||
      OutputPin::kTrue == state;
}

bool IsLow(OutputPin::PinState const state) {
  return !IsHigh(state);
}

class InputPin {
 public:
  InputPin() : wire_(0) {}

  void setWire(Wire* wire) { wire_ = wire; }
  Wire* wire() const { return wire_; }
 private:
  Wire* wire_;
};

#endif  // INCLUDED_PIN_H_
