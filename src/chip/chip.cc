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



#include <iostream>


int _tmain(int argc, _TCHAR* argv[]) {
  WorkBench bench;

  NandBuilder<3>* nand_builder = new NandBuilder<3>();
  FixedLowBuilder* low_builder = new FixedLowBuilder();
  NotBuilder* not_builder = new NotBuilder();
  bench.RegisterBuilder(nand_builder);
  bench.RegisterBuilder(low_builder);
  bench.RegisterBuilder(not_builder);

  NandChip<3> *nand = nand_builder->CreateInstance();
  FixedLowChip *low = low_builder->CreateInstance();
  CompositeChip *not = not_builder->CreateInstance();

  Board board;
  board.AddChip(nand);
  board.AddChip(low);
  board.AddChip(not);

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

