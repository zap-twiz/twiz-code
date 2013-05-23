#ifndef INCLUDED_COMPOSITE_CHIP_BUILDER_H_
#define INCLUDED_COMPOSITE_CHIP_BUILDER_H_

#include "chip/chip.h"
#include "chip/chip_description.h"
#include "chip/work_bench.h"

#include <string>
#include <utility>

class CompositeChipBuilder : public ChipBuilder {
 public:
  CompositeChipBuilder() : description_(NULL) {}
  virtual ~CompositeChipBuilder() {}

  void set_description(ChipDescription const * description) { description_.reset(description); }
  virtual ChipDescription const * description() const { return description_.get(); }

  //WorkBench* bench() { return &bench_; }

  virtual Chip* CreateInstance() { return NULL; }

 private:
   std::auto_ptr<ChipDescription const> description_;
};

class ChipBuildInstruction {
 public:
  virtual ~ChipBuildInstruction() {}

  virtual void DoEvaluate(WorkBench const & bench, Board* board) = 0;
};

class AddChipInstruction : public ChipBuildInstruction {
 public:
   AddChipInstruction(std::string const & name,
                      std::string const & type)
    : name_(name), type_(type) {}
  virtual ~AddChipInstruction() {}

  virtual void DoEvaluate(WorkBench const & bench, Board* board);
 private:
  std::string name_, type_;
};

void AddChipInstruction::DoEvaluate(WorkBench const & bench, Board* board) {
  ChipBuilder* builder = bench.builder(type_);
  Chip* chip = builder->CreateInstance();
  chip->set_name(name_);
  board->AddChip(chip);
}

class AddWireInstruction : public ChipBuildInstruction {
 public:
  AddWireInstruction(std::string const & name) : name_(name) {}
  virtual ~AddWireInstruction() {}

  virtual void DoEvaluate(WorkBench const & bench, Board* board) {
    Wire* wire = board->AddWire(new Wire(name_));
  }
 private:
  std::string name_;
};

class ConnectWireInstruction : public ChipBuildInstruction {
 public:
   ConnectWireInstruction(std::string const & wire_name,
                          std::string const & output_chip_name,
                          std::string const & output_pin_name,
                          std::string const & input_chip_name,
                          std::string const & input_pin_name)
    : wire_name_(wire_name),
      output_chip_name_(output_chip_name),
      output_pin_name_(output_pin_name),
      input_chip_name_(input_chip_name),
      input_pin_name_(input_pin_name) {}
  virtual ~ConnectWireInstruction() {}

  virtual void DoEvaluate(WorkBench const & bench, Board* board) {
    //wire_->setSource(output_pin_);
    //wire_->addConnection(input_pin_);
  }

 private:
  std::string const wire_name_,
    output_chip_name_,
    output_pin_name_,
    input_chip_name_,
    input_pin_name_;
};

#include <assert.h>

class ConnectWireToInputInstruction : public ChipBuildInstruction {
 public:
   ConnectWireToInputInstruction(std::string const & wire_name,
                                 std::string const & input_chip_name,
                                 std::string const & input_pin_name)
    : wire_name_(wire_name),
      input_chip_name_(input_chip_name),
      input_pin_name_(input_pin_name) {}
  virtual ~ConnectWireToInputInstruction() {}

  virtual void DoEvaluate(WorkBench const & bench, Board* board) {
    assert(false);

    // TODO implement
  }

 private:
  std::string const wire_name_,
    input_chip_name_,
    input_pin_name_;
};


#endif  // INCLUDED_COMPOSITE_CHIP_BUILDER_H_
