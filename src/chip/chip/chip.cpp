// chip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "wire.h"
#include "pin.h"

#include <string>
#include <vector>
#include <algorithm>

#include "chip.h"
#include "chip_description.h"
#include "named_element.h"
#include "work_bench.h"

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

class FixedLowChip : public Chip {
 public:
  FixedLowChip(ChipDescription const* description) : Chip(description) {
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
    return getDescription();
  }
 private:
  static ChipDescription* getDescription();
};

ChipDescription* FixedLowBuilder::getDescription() {
  static ChipDescription* k_description = NULL;
  if (!k_description) {
    std::string name = std::string("FixedLow");
    k_description = ChipDescription::Create(
      name,
      std::vector<std::string>(),
      CreateStrings("output", 1));
  }
  return k_description;
}

FixedLowChip* FixedLowBuilder::CreateInstance() {
  FixedLowChip *instance = new FixedLowChip(description());
  return instance;
}

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

  Chip* GetChipByName(std::string const & name) {
    std::vector<Chip*>::iterator iter(chips_.begin()), end(chips_.end());
    for (; iter != end; ++iter) {
      if ((*iter)->name() == name) {
        return *iter;
      }
    }

    return NULL;
  }

  Wire* AddWire() {
    Wire* wire = new Wire();
    wires_.push_back(wire);
    return wire;
  }
  std::vector<Wire*> const & wires() const { return wires_; }

  Wire* GetWireByName(std::string const & name) {
    std::vector<Wire*>::iterator iter(wires_.begin()), end(wires_.end());
    for (; iter != end; ++iter) {
      if ((*iter)->name() == name) {
        return *iter;
      }
    }
    return NULL;
  }

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
};

class CompositeChip : public Chip {
 public:
  CompositeChip(ChipDescription const * description);
  virtual ~CompositeChip() {}

  Board& board() { return board_; }
  Board const & board() const { return board_; }

  OutputPin* input_pin_adapter(int pin) {
    return &input_pin_adapters_[pin];
  }

  InputPin* output_pin_adapter(int pin) {
    return &output_pin_adapters_[pin];
  }

 private:

  std::vector<InputPin> output_pin_adapters_;
  std::vector<OutputPin> input_pin_adapters_;

  Board board_;
};

CompositeChip::CompositeChip(ChipDescription const * description) : Chip(description) {
  addInputPins(description->num_input_ports());
  addOutputPins(description->num_output_ports());

  output_pin_adapters_.resize(description->num_output_ports());
  input_pin_adapters_.resize(description->num_input_ports());
}

class CompositeChipBuilder : public ChipBuilder {
 public:
  CompositeChipBuilder(ChipDescription const* description,
                       WorkBench& bench)
    : description_(description),
      bench_(bench) {}
  virtual ~CompositeChipBuilder() {
    delete description_;
  }

  virtual ChipDescription const * description() const { return description_; }

  WorkBench* bench() { return &bench_; }

 private:
  ChipDescription const * description_;
  WorkBench& bench_;
};

class ChipBuildInstruction {
 public:
  virtual ~ChipBuildInstruction() {}

  virtual void DoEvaluate(WorkBench& bench, Board* board) = 0;
};

class AddChipInstruction : public ChipBuildInstruction {
 public:
   AddChipInstruction(std::string const & name,
                      std::string const & type)
    : name_(name), type_(type) {}
  virtual ~AddChipInstruction() {}

  virtual void DoEvaluate(WorkBench& bench, Board* board);
 private:
  std::string name_, type_;
};

void AddChipInstruction::DoEvaluate(WorkBench& bench, Board* board) {
  ChipBuilder* builder = bench.builder(type_);
  Chip* chip = builder->CreateInstance();
  chip->set_name(name_);
  board->AddChip(chip);
}

class AddWireInstruction : public ChipBuildInstruction {
 public:
  AddWireInstruction(std::string const & name) : name_(name) {}
  virtual ~AddWireInstruction() {}

  virtual void DoEvaluate(WorkBench& bench, Board* board) {
    Wire* wire = board->AddWire();
    wire->set_name(name_);
  }
 private:
  std::string name_;
};

class ConnectWireInstruction : public ChipBuildInstruction {
 public:
   ConnectWireInstruction(std::string const & wire,
                          std::string const & output_chip,
                          std::string const & output_pin,
                          std::string const & input_chip,
                          std::string const & input_pin)
    : wire_(wire),
      output_chip_(output_chip),
      output_pin_(output_pin),
      input_chip_(input_chip),
      input_pin_(input_pin) {
   }
  virtual ~ConnectWireInstruction() {}

  virtual void DoEvaluate(WorkBench& bench, Board* board);

 private:
  std::string wire_;
  std::string output_chip_;
  std::string output_pin_;
  std::string input_chip_;
  std::string input_pin_;
};

void ConnectWireInstruction::DoEvaluate(WorkBench& bench, Board* board) {
  Wire* wire = board->GetWireByName(wire_);
  Chip *output = board->GetChipByName(output_chip_);
  Chip *input = board->GetChipByName(input_chip_);

  InputPin *input_pin = input->GetInputPinByName(input_pin_);
  OutputPin *output_pin = output->GetOutputPinByName(output_pin_);

  wire->setSource(output_pin);
  wire->addConnection(input_pin);
}

class ConnectInternalPinInstruction : public ChipBuildInstruction {
 public:
   ConnectInternalPinInstruction(std::string const & wire,
                                 std::string const & input_chip,
                                 std::string const & input_pin,
                                 int internal_pin,
                                 bool is_output)
    : wire_(wire),
      input_chip_(input_chip),
      input_pin_(input_pin),
      internal_pin_(internal_pin),
      is_output_(is_output) {
   }

  virtual ~ConnectInternalPinInstruction() {}

  virtual void DoEvaluate(WorkBench& bench, Board* board);

 private:
  std::string wire_;
  std::string input_chip_;
  std::string input_pin_;
  int internal_pin_;
  bool is_output_;
};

class NotBuilder : public ChipBuilder {
 public:
  virtual ~NotBuilder() {}

  virtual CompositeChip* CreateInstance();

  virtual ChipDescription const * description() const {
    return GetDescription();
  }
 private:
   static ChipDescription* GetDescription();
};

CompositeChip* NotBuilder::CreateInstance() {
  //FixedLowChip *instance = new FixedLowChip(description());
  return NULL;//instance;
}

ChipDescription* NotBuilder::GetDescription() {
  static ChipDescription* k_description = NULL;
  if (!k_description) {
    std::string name = std::string("Not");
    k_description = ChipDescription::Create(
      name,
      std::vector<std::string>(),
      CreateStrings("output", 1));
  }
  return k_description;
}

class ProgrammableChipBuilder : public CompositeChipBuilder {
 public:
  ProgrammableChipBuilder(ChipDescription const * description,
                          WorkBench& bench)
    : CompositeChipBuilder(description, bench) {}

  virtual ~ProgrammableChipBuilder();

  void PushInstruction(ChipBuildInstruction* instruction) {
    operation_stack_.push_back(instruction);
  }

  virtual CompositeChip* CreateInstance();

 private:
  std::vector<ChipBuildInstruction*> operation_stack_;
};

ProgrammableChipBuilder::~ProgrammableChipBuilder() {
  std::vector<ChipBuildInstruction*>::iterator iter(operation_stack_.begin()),
      end(operation_stack_.end());
  for (; iter != end; ++iter) {
    delete *iter;
  }
}

CompositeChip* ProgrammableChipBuilder::CreateInstance() {
  CompositeChip* chip = new CompositeChip(description());
  Board& board = chip->board();

  WorkBench* work_bench = bench();

  std::vector<ChipBuildInstruction*>::iterator iter(operation_stack_.begin()),
    end(operation_stack_.end());
  for (; iter != end; ++iter) {
    (*iter)->DoEvaluate(*work_bench, &board);
  }

  return chip;
}

#include <iostream>

int _tmain(int argc, _TCHAR* argv[]) {
  WorkBench bench;

  NandBuilder<3>* nand_builder = new NandBuilder<3>();
  NandBuilder<2>* nand_builder2 = new NandBuilder<2>();
  FixedLowBuilder* low_builder = new FixedLowBuilder();
  NotBuilder* not_builder = new NotBuilder();
  bench.RegisterBuilder(nand_builder);
  bench.RegisterBuilder(nand_builder2);
  bench.RegisterBuilder(low_builder);
  bench.RegisterBuilder(not_builder);

  NandChip<3> *nand = nand_builder->CreateInstance();
  FixedLowChip *low = low_builder->CreateInstance();
  CompositeChip *not = not_builder->CreateInstance();

  ChipDescription const * description = not_builder->description();
  ProgrammableChipBuilder *composite_builder = new ProgrammableChipBuilder(description, bench);

  composite_builder->PushInstruction(new AddChipInstruction("nand", "Nand2"));
  composite_builder->PushInstruction(new AddWireInstruction("input"));
  composite_builder->PushInstruction(new AddChipInstruction("low", "FixedLow"));
  composite_builder->PushInstruction(new AddWireInstruction("unnamed"));
  composite_builder->PushInstruction(new ConnectWireInstruction("unnamed",
                                                                "low",
                                                                "output0",
                                                                "nand",
                                                                "input0"));

  CompositeChip *programmableNot = composite_builder->CreateInstance();


  Board board;
  board.AddChip(nand);
  board.AddChip(low);
  board.AddChip(not);
  board.AddChip(programmableNot);

  Wire *w1 = board.AddWire(),
      *w2 = board.AddWire(),
      *w3 = board.AddWire();
  Wire *output = board.AddWire();

  w1->setSource(&low->output_pins()[0]);
  w2->setSource(&low->output_pins()[0]);
  w3->setSource(&low->output_pins()[0]);

  w1->addConnection(&nand->input_pins()[0]);
  w2->addConnection(&nand->input_pins()[1]);
  w3->addConnection(&nand->input_pins()[2]);

  output->setSource(&nand->output_pins()[0]);

  nand->evaluate();

  //cout << nand->output_pins()[0].state() == 

  return 0;
}

