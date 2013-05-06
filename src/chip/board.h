#ifndef INCLUDED_BOARD_H_
#define INCLUDED_BOARD_H_

#include "chip/chip.h"
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
    return chip;
  }
  std::vector<Chip*> const & chips() const { return chips_; }
  Chip* chip(std::string const & name) const;

  Wire* AddWire() {
    Wire* wire = new Wire();
    wires_.push_back(wire);
    return wire;
  }
  std::vector<Wire*> const & wires() const { return wires_; }
  Wire* wire(std::string const & name) const;
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
  }

  std::vector<Chip*> chips_;
  std::vector<Wire*> wires_;

  typedef std::map<std::string, Wire*> WireInstanceMap;
  typedef std::map<std::string, Chip*> ChipInstanceMap;

  WireInstanceMap wire_instance_map_;
  ChipInstanceMap chip_instance_map_;
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


#endif
