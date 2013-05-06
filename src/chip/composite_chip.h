#ifndef INCLUDED_COMPOSITE_CHIP_H_
#define INCLUDED_COMPOSITE_CHIP_H_

#include "chip/board.h"
#include "chip/chip.h"
#include "chip/chip_description.h"

class CompositeChip : public Chip {
 public:
  CompositeChip(ChipDescription const & description);
  virtual ~CompositeChip() {}

  Board& board() { return board_; }
  Board const & board() const { return board_; }
 private:

  Board board_;
};

CompositeChip::CompositeChip(ChipDescription const & description)
  : Chip(description) {
  addInputPins(description.num_input_ports());
  addOutputPins(description.num_output_ports());
}

#endif
