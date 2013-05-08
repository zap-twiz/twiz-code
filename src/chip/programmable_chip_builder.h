#ifndef INCLUDED_PROGRAMMABLE_CHIP_BUILDER_H_
#define INCLUDED_PROGRAMMABLE_CHIP_BUILDER_H_

#include "chip/chip_description.h"
#include "chip/composite_chip.h"
#include "chip/composite_chip_builder.h"
#include "chip/work_bench.h"

#include <vector>

class ProgrammableChipBuilder : public CompositeChipBuilder {
 public:
  ProgrammableChipBuilder(WorkBench const & bench)
    : work_bench_(bench) {
  }

  virtual ~ProgrammableChipBuilder();

  void PushInstruction(ChipBuildInstruction* instruction) {
    operation_stack_.push_back(instruction);
  }

  virtual CompositeChip* CreateInstance();

 private:
  std::vector<ChipBuildInstruction*> operation_stack_;

  WorkBench const& work_bench_;
};

ProgrammableChipBuilder::~ProgrammableChipBuilder() {
  std::vector<ChipBuildInstruction*>::iterator iter(operation_stack_.begin()),
      end(operation_stack_.end());
  for (; iter != end; ++iter) {
    delete *iter;
  }
}

CompositeChip* ProgrammableChipBuilder::CreateInstance() {
  CompositeChip* chip = new CompositeChip(*description());
  Board& board = chip->board();

#if 0
  WorkBench* bench = bench();

  std::vector<ChipBuildInstruction*>::iterator iter(operation_stack_.begin()),
    end(operation_stack_.end());
  for (; iter != end; ++iter) {
    iter->DoEvaluate(*bench, board);
  }
#endif

  return chip;
}

#endif  // INCLUDED_PROGRAMMABLE_CHIP_BUILDER_H_
