// lex.cpp : Defines the entry point for the console application.
//

#include "base/trie.h"

#include <string>

#include <fstream>
#include <iostream>

#include "streams/stream.h"
#include <map>
#include "streams/counter.h"
#include "lexer/token.h"
#include "lexer/line_counting_stream.h"
#include "lexer/lex_utils.h"

#include "chip.cc"
#include "streams/iostream.h"

int main(int argc, char* argv[])
{
#if 0
  std::ifstream fstream("example.thd");

  if (!fstream) { 
    return 0;
  }

  IoStream io_stream(&fstream);

  LineCountingStream line_count(io_stream);
  size_t current_count = line_count.GetCount();

  TokenStream token_stream(line_count);

  while (!token_stream.IsEOS()) {
    Token token = token_stream.Get();
    std::cout << "Token:= " << Token::kTokenTypeNames[token.type()] << ": "
        << token.value() << ": Line :" << token.line() << std::endl;
  }
#endif
#if 0
  while (!line_count.IsEOS()) {
    std::cout << line_count.Get();
    if (current_count != line_count.GetCount()) {
      current_count = line_count.GetCount();
      std::cout << current_count << " :";
    }
  }
#endif

#if 0
  NandChip<5> nand_chip;
  Wire a, b, c, d, e;
  Wire output;

  nand_chip.input_pin(0).set_wire(&a);
  nand_chip.input_pin(1).set_wire(&b);
  nand_chip.input_pin(2).set_wire(&c);
  nand_chip.input_pin(3).set_wire(&d);
  nand_chip.input_pin(4).set_wire(&d);

  nand_chip.output_pin(0).set_wire(&output);

  a.value() = Wire::HIGH;
  b.value() = Wire::HIGH;
  c.value() = Wire::HIGH;
  d.value() = Wire::LOW;
  e.value() = Wire::HIGH;

  nand_chip.ApplyInputs();

  Wire::State output_value = nand_chip.output_pin(0).wire()->value();
#endif

  return 0;
}

