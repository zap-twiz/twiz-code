#ifndef INCLUDED_BOARD_H_
#define INCLUDED_BOARD_H_

#include "chip/chip.h"
#include "chip/pin.h"
#include "chip/wire.h"

#include <map>
#include <vector>

class Board {
 public:
  Board() {}
  virtual ~Board() {
    destroy();
  }

  Chip* AddChip(Chip* chip) {
    chips_.push_back(chip);
    chip_instance_map_[chip->name()] = chip;
    return chip;
  }
  std::vector<Chip*> const & chips() const { return chips_; }
  Chip* chip(std::string const & name) const;

  Wire* AddWire(Wire* wire) {
    wires_.push_back(wire);
    wire_instance_map_[wire->name()] = wire;
    return wire;
  }
  std::vector<Wire*> const & wires() const { return wires_; }
  Wire* wire(std::string const & name) const;

  InputPin* AddInputPin(InputPin* pin) {
    input_pins_.push_back(pin);
    input_pin_instance_map_[pin->name()] = pin;
    return pin;
  }
  std::vector<InputPin*> const & input_pins() const { return input_pins_; }
  InputPin* input_pin(std::string const & name) const;

  OutputPin* AddOutputPin(OutputPin* pin) {
    output_pins_.push_back(pin);
    output_pin_instance_map_[pin->name()] = pin;
    return pin;
  }
  std::vector<OutputPin*> const & output_pins() const { return output_pins_; }
  OutputPin* output_pin(std::string const & name) const;

  Pin* pin(std::string const &name) const;

 private:
  void destroy() {
    {
      std::vector<Wire*>::iterator iter(wires_.begin()), end(wires_.end());
      for (; iter != end; ++iter) {
        delete *iter;
      }
    }
    {
      std::vector<Chip*>::iterator iter(chips_.begin()), end(chips_.end());
      for (; iter != end; ++iter) {
        delete *iter;
      }
    }
    {
      std::vector<InputPin*>::iterator  iter(input_pins_.begin()),
        end(input_pins_.end());
      for (; iter != end; ++iter) {
        delete *iter;
      }
    }
    {
      std::vector<OutputPin*>::iterator iter(output_pins_.begin()),
        end(output_pins_.end());
      for (; iter != end; ++iter) {
        delete *iter;
      }
    }
  }

  std::vector<Chip*> chips_;
  std::vector<Wire*> wires_;
  std::vector<InputPin*> input_pins_;
  std::vector<OutputPin*> output_pins_;

  typedef std::map<std::string, Wire*> WireInstanceMap;
  typedef std::map<std::string, Chip*> ChipInstanceMap;
  typedef std::map<std::string, InputPin*> InputPinInstanceMap;
  typedef std::map<std::string, OutputPin*> OutputPinInstanceMap;

  WireInstanceMap wire_instance_map_;
  ChipInstanceMap chip_instance_map_;
  InputPinInstanceMap input_pin_instance_map_;
  OutputPinInstanceMap output_pin_instance_map_;
};

Chip* Board::chip(std::string const & name) const {
  ChipInstanceMap::const_iterator find(chip_instance_map_.find(name));
  if (find != chip_instance_map_.end()) {
    return find->second;
  }
  return NULL;
}

Wire* Board::wire(std::string const & name) const {
  WireInstanceMap::const_iterator find(wire_instance_map_.find(name));
  if (find != wire_instance_map_.end()) {
    return find->second;
  }
  return NULL;
}

InputPin* Board::input_pin(std::string const & name) const {
  InputPinInstanceMap::const_iterator find(input_pin_instance_map_.find(name));
  if (find != input_pin_instance_map_.end()) {
    return find->second;
  }
  return NULL;
}

OutputPin* Board::output_pin(std::string const & name) const {
  OutputPinInstanceMap::const_iterator find(output_pin_instance_map_.find(name));
  if (find != output_pin_instance_map_.end()) {
    return find->second;
  }
  return NULL;
}

Pin* Board::pin(std::string const & name) const {
  Pin *pin = input_pin(name);

  if (!pin) {
    pin = output_pin(name);
  }

  return pin;
}


#endif
