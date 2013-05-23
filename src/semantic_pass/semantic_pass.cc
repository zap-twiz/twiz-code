
#include "parser/parse_error_collection.h"
#include "parser/parser.h"

#include "base/string_utils.h"
#include "lexer/line_counting_stream.h"
#include "lexer/token_stream.h"
#include "streams/iostream.h"

#include "chip/chip_description.h"
#include "chip/programmable_chip_builder.h"

#include <fstream>

#include <assert.h>

#if 0
    THD_MODULE,
    IDENTIFIER_DEFINITION,
    IDENTIFIER_DEFINITION_LIST,
    IDENTIFIER_REFERENCE,
    IDENTIFIER_REFERENCE_LIST,
    CHIP_REFERENCE,
    CHIP_REFERENCE_LIST,
    CHIP_INSTANCE,
    WIRE_INSTANCE,
    LEFT_ASSIGN_STATEMENT,
    RIGHT_ASSIGN_STATEMENT,
    CHIP_BODY,
    LVALUE_LIST,
    LVALUE,
    RVALUE_LIST,
    RVALUE,
    IMMEDIATE_VALUE,
    CHIP_DEFINITION,
    SINGLE_PIN_DEFINITION,
    IMPORT_STATEMENT,

    NUMBER_RANGE,
    NUMBER_COLLECTION,
    NUMBER,
#endif

class SemanticError {
 public:
  SemanticError(ParseNode const & parse_node, std::string const & message)
    : parse_node_(parse_node), message_(message) {}

  ParseNode const & token() const { return parse_node_; }
  std::string const & message() const { return message_; }

 private:
  ParseNode const & parse_node_;
  std::string message_;

  SemanticError();
};

int TokenToInt(Token const & token) {
  int value = 0;
  switch(token.type()) {
    case Token::NUMBER_BINARY:
      value = BinaryToInt(token.value());
      break;
    case Token::NUMBER_DECIMAL:
      value = DecimalToInt(token.value());
      break;
    case Token::NUMBER_HEX:
      value = HexToInt(token.value());
      break;
    default:
      assert(false);
      break;
  }
  return value;
}

class ParseNodeAdapter {
 public:
  virtual ~ParseNodeAdapter() {}

  virtual void DoEvaluate(ParseNode const & parse_node) = 0;
};

class NumberAdapter : public ParseNodeAdapter {
 public:
  virtual void DoEvaluate(ParseNode const & parse_node) {
    assert(ParseNode::NUMBER == parse_node.type());
    value_ = TokenToInt(parse_node.terminals()[0].first);
  }
  int value() const { return value_; }
 private:
  int value_;
};

class NumberRangeAdapter : public ParseNodeAdapter {
 public:
  virtual void DoEvaluate(ParseNode const & parse_node) {
    assert(ParseNode::NUMBER_RANGE == parse_node.type());
    NumberAdapter min, max;
    min.DoEvaluate(*parse_node.non_terminals()[0].first);
    max.DoEvaluate(*parse_node.non_terminals()[1].first);

    min_ = min.value();
    max_ = max.value();
  }

  int min() const { return min_; }
  int max() const { return max_; }

  void AppendToVector(std::vector<int>* value) {
    for (int x = min_; x <= max_; ++x) {
      value->push_back(x);
    }
  }
 private:
  int min_, max_;
};

class NumberCollectionAdapter : public ParseNodeAdapter {
 public:
  virtual void DoEvaluate(ParseNode const & parse_node);

  std::vector<int> const & collection() const { return collection_; }
 private:
  std::vector<int> collection_;
};

void NumberCollectionAdapter::DoEvaluate(ParseNode const & parse_node) {
  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();

  collection_.clear();
  ParseNode::NonTerminalArray::const_iterator iter(non_terminals.begin()),
      end(non_terminals.end());
  for (; iter != end; ++iter) {
    ParseNode const & non_terminal = *iter->first;
    switch (non_terminal.type()) {
      case ParseNode::NUMBER:
        {
          NumberAdapter adapter;
          adapter.DoEvaluate(non_terminal);
          collection_.push_back(adapter.value());
        }
        break;
      case ParseNode::NUMBER_RANGE:
        {
          NumberRangeAdapter adapter;
          adapter.DoEvaluate(non_terminal);
          adapter.AppendToVector(&collection_);
        }
        break;
      default:
        assert(false);
        break;
    }
  }
}

#include <sstream>

class IdentifierDefinition {
 public:
  std::string name;
  int array_size;
};

void AppendNamesFromDefinition(IdentifierDefinition const & definition,
                               std::vector<std::string>* names) {

  if (definition.array_size != 1) {
    std::vector<std::string> output_names = CreateStrings(definition.name,
                                                          definition.array_size);

    std::vector<std::string>::iterator iter(output_names.begin()),
        end(output_names.end());
    for (; iter != end; ++iter) {
      names->push_back(*iter);
    }
  } else {
    names->push_back(definition.name);
  }
}


class IdentifierDefinitionAdapter : public ParseNodeAdapter {
 public:
  virtual void DoEvaluate(ParseNode const & parse_node);

  IdentifierDefinition const & definition() const { return definition_; }
 private:
  IdentifierDefinition definition_;
};

void IdentifierDefinitionAdapter::DoEvaluate(ParseNode const & parse_node) {
  definition_.name = parse_node.terminals()[0].first.value();
  switch (parse_node.type()) {
    case ParseNode::SINGLE_PIN_DEFINITION:
      definition_.array_size = 1;
      break;
    case ParseNode::IDENTIFIER_DEFINITION:
      {
        NumberAdapter adapter;
        adapter.DoEvaluate(*parse_node.non_terminals()[0].first);
        definition_.array_size = adapter.value();
      }
      break;
    default:
      assert(false);
      break;
  }
}

class IdentifierDefinitionListAdapter : public ParseNodeAdapter {
 public:
  virtual void DoEvaluate(ParseNode const & parse_node);
  std::vector<IdentifierDefinition> const & definition_list() const { return definition_list_; }
 private:
  std::vector<IdentifierDefinition> definition_list_;
};

void IdentifierDefinitionListAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(parse_node.type() == ParseNode::IDENTIFIER_DEFINITION_LIST ||
         parse_node.type() == ParseNode::IDENTIFIER_DEFINITION);
  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();
  ParseNode::NonTerminalArray::const_iterator iter(non_terminals.begin()),
    end(non_terminals.end());

  definition_list_.clear();
  for (; iter != end; ++iter) {
    IdentifierDefinitionAdapter adapter;
    adapter.DoEvaluate(*iter->first);
    definition_list_.push_back(adapter.definition());
  }
}

#include "chip/composite_chip.h"
#include "chip/composite_chip_builder.h"
#include "chip/work_bench.h"

class WireInstanceAdapter : public ParseNodeAdapter {
 public:
   WireInstanceAdapter(WorkBench const & work_bench,
                       CompositeChip& chip,
                       ProgrammableChipBuilder& chip_builder)
    : work_bench_(work_bench),
      chip_(chip),
      chip_builder_(chip_builder) {}

   virtual void DoEvaluate(ParseNode const & parse_node);
 private:
  WorkBench const & work_bench_;
  CompositeChip& chip_;
  ProgrammableChipBuilder& chip_builder_;
};

void WireInstanceAdapter::DoEvaluate(ParseNode const & parse_node) {
  /*
  if (!context.ConsumeToken(Token::WIRE))
  if (!context.ConsumeNonTerminal(EvalIdentifierList))
  if (!context.ConsumeToken(Token::SEMI_COLON))
  */

  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();

  IdentifierDefinitionListAdapter adapter;
  adapter.DoEvaluate(*non_terminals[0].first);

  Board & board = chip_.board();

  std::vector<IdentifierDefinition> const & definitions(adapter.definition_list());
  std::vector<IdentifierDefinition>::const_iterator iter(definitions.begin()),
    end(definitions.end());
  for (; iter != end; ++iter) {
    std::vector<std::string> chip_names;
    AppendNamesFromDefinition(*iter, &chip_names);

    std::vector<std::string>::const_iterator name_iter(chip_names.begin()),
      name_end(chip_names.end());
    for (; name_iter != name_end; ++name_iter) {
      if (board.wire(*name_iter) != NULL) {
        assert(false);
        // TODO:  Log semantic error - duplicate identifier
      }
      board.AddWire(new Wire(*name_iter));

      chip_builder_.PushInstruction(new AddWireInstruction(*name_iter));
    }
  }
}

class ChipInstanceAdapter : public ParseNodeAdapter {
 public:
  ChipInstanceAdapter(WorkBench const & work_bench,
                      CompositeChip& chip,
                      ProgrammableChipBuilder& chip_builder)
    : work_bench_(work_bench),
      chip_(chip),
      chip_builder_(chip_builder) {}

  virtual void DoEvaluate(ParseNode const & parse_node);
 private:
  WorkBench const & work_bench_;
  CompositeChip& chip_;
  ProgrammableChipBuilder& chip_builder_;
};

void ChipInstanceAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(parse_node.type() == ParseNode::CHIP_INSTANCE);
  /*
  if (!context.ConsumeToken(Token::CHIP))
  if (!context.ConsumeToken(Token::IDENTIFIER))
  if (!context.ConsumeNonTerminal(EvalIdentifierList))
  if (!context.ConsumeToken(Token::SEMI_COLON))
  */
  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();

  std::string const & chip_type = parse_node.terminals()[1].first.value();

  ChipBuilder* builder = work_bench_.builder(chip_type);
  if (!builder) {
    assert(false);
    // Log a semantic error.  Attempt to build chip that has undefined type.
  }

  IdentifierDefinitionListAdapter adapter;
  adapter.DoEvaluate(*non_terminals[0].first);

  Board & board = chip_.board();

  std::vector<IdentifierDefinition> const & definitions(adapter.definition_list());
  std::vector<IdentifierDefinition>::const_iterator iter(definitions.begin()),
    end(definitions.end());
  for (; iter != end; ++iter) {
    std::vector<std::string> chip_names;
    AppendNamesFromDefinition(*iter, &chip_names);

    std::vector<std::string>::const_iterator name_iter(chip_names.begin()),
      name_end(chip_names.end());
    for (; name_iter != name_end; ++name_iter) {
      if (board.chip(*name_iter) != NULL) {
        assert(false);
        // TODO:  Log semantic error - duplicate identifier
      }
      Chip* new_chip = builder->CreateInstance();
      new_chip->set_name(*name_iter);
      board.AddChip(new_chip);

      chip_builder_.PushInstruction(new AddChipInstruction(*name_iter,
                                                           chip_type));
    }
  }
}

struct IdentifierReference {
  IdentifierReference() : pin(NULL), chip(NULL), wire(NULL) {}
  Pin* pin;
  Chip* chip;
  Wire* wire;
  Token::TokenType immediate;  // HIGH, LOW, TRUE, FALSE
};

class IdentifierReferenceAdapter : public ParseNodeAdapter {
 public:
  IdentifierReferenceAdapter(CompositeChip const & chip,
                             std::vector<IdentifierReference>& references);
  virtual void DoEvaluate(ParseNode const & parse_node);

  std::vector<IdentifierReference> const & references() const { return references_; }

 private:
   void GetChipAndPinNames(ParseNode const & parse_node,
                           std::vector<std::string>& chip_names,
                           std::vector<std::string>& pin_names);

  CompositeChip const & chip_;

  std::vector<IdentifierReference>& references_;
};

IdentifierReferenceAdapter::IdentifierReferenceAdapter(
    CompositeChip const & chip,
    std::vector<IdentifierReference>& references)
  : chip_(chip),
    references_(references) {
}

std::vector<std::string> ConstructArrayReferenceStrings(
    std::string const & prefix,
    std::vector<int> const & array_references) {

  std::vector<std::string> result;
  std::vector<int>::const_iterator array_iter(array_references.begin()),
      array_end(array_references.end());
  for (; array_iter != array_end; ++array_iter) {
    std::ostringstream s;
    s << prefix << '[' << *array_iter << ']';
    result.push_back(s.str());
  }

  return result;
}

void IdentifierReferenceAdapter::GetChipAndPinNames(
    ParseNode const & parse_node,
    std::vector<std::string>& chip_names,
    std::vector<std::string>& pin_names) {
  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();
  ParseNode::TerminalArray const & terminals = parse_node.terminals();

  std::string const & identifier_name = terminals[0].first.value();
  chip_names.push_back(identifier_name);

  int token_offset = 1;
  int non_terminal_offset = 0;

  if (terminals[token_offset].first.type() == Token::LEFT_SQUARE_BRACE) {
    NumberCollectionAdapter adapter;
    adapter.DoEvaluate(*non_terminals[non_terminal_offset].first);

    chip_names = ConstructArrayReferenceStrings(identifier_name,
                                                adapter.collection());
    token_offset+=2;
    ++non_terminal_offset;
  }

  if (terminals[token_offset].first.type() == Token::DOT) {
    ++token_offset;

    std::string const & pin_name = terminals[token_offset].first.value();

    if (terminals[token_offset].first.type() == Token::LEFT_SQUARE_BRACE) {
      NumberCollectionAdapter pin_number_adapter;
      pin_number_adapter.DoEvaluate(*non_terminals[non_terminal_offset].first);

      pin_names = ConstructArrayReferenceStrings(pin_name,
                                                 pin_number_adapter.collection());
    }
  }

}

void IdentifierReferenceAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(ParseNode::IDENTIFIER_REFERENCE == parse_node.type());
  /*
  if (!context.ConsumeToken(Token::IDENTIFIER))

  if (context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
    if (!context.ConsumeNonTerminal(EvalNumberList))
    if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE))
  }

  if (context.ConsumeToken(Token::DOT)) {
    if (!context.ConsumeToken(Token::IDENTIFIER))
    if (context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
      if (!context.ConsumeNonTerminal(EvalNumberList))
      if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE))
    }
  }
  */
  std::vector<std::string> chip_names, pin_names;
  GetChipAndPinNames(parse_node, chip_names, pin_names);

  Board const & board = chip_.board();

  if (pin_names.empty()) {
    // The chip names must be references to either chips, wires, or pins
    std::vector<std::string>::const_iterator identifier_iter(chip_names.begin()),
      identifier_end(chip_names.end());
    for (; identifier_iter != identifier_end; ++identifier_iter) {
      IdentifierReference reference;
      reference.chip = board.chip(*identifier_iter);

      if (NULL != reference.chip) {
        references_.push_back(reference);
        continue;
      }

      reference.wire = board.wire(*identifier_iter);
      if (NULL != reference.wire) {
        references_.push_back(reference);
        continue;
      }

      reference.pin = board.pin(*identifier_iter);
      if (NULL != reference.pin) {
        references_.push_back(reference);
        continue;
      }

      // Not reached
      assert(false);
    }
  } else {
    // Reference must be a pin on a chip
    std::vector<std::string>::const_iterator identifier_iter(chip_names.begin()),
      identifier_end(chip_names.end());
    for (; identifier_iter != identifier_end; ++identifier_iter) {
      Chip* chip = board.chip(*identifier_iter);

      if (!chip) {
        // semantic error
        assert(false);
      }

      ChipDescription const & description = chip->description();

      std::vector<std::string>::const_iterator pin_iter(pin_names.begin()),
        pin_end(pin_names.end());
      for (; pin_iter != pin_end; ++pin_iter) {
        int pin_index = description.has_input_port(*pin_iter);
        IdentifierReference reference;

        if (-1 != pin_index) {
          reference.pin = &chip->input_pins()[pin_index];
          references_.push_back(reference);
          continue;
        }

        pin_index = description.has_output_port(*pin_iter);
        if (-1 != pin_index) {
          reference.pin = &chip->output_pins()[pin_index];
          references_.push_back(reference);
          continue;
        }

        // semantic error
        assert(false);
      }
    }
  }
}

class IdentifierReferenceListAdapter : public ParseNodeAdapter {
 public:
  IdentifierReferenceListAdapter(CompositeChip const & chip,
                                 std::vector<IdentifierReference>& references)
    : chip_(chip),
      references_(references) {}

  virtual void DoEvaluate(ParseNode const & parse_node);

  std::vector<IdentifierReference> const & references() const { return references_; }
 private:
  CompositeChip const & chip_;
  std::vector<IdentifierReference>& references_;
};

void IdentifierReferenceListAdapter::DoEvaluate(const ParseNode &parse_node) {
  assert(ParseNode::IDENTIFIER_REFERENCE_LIST == parse_node.type());

  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();
  ParseNode::NonTerminalArray::const_iterator iter(non_terminals.begin()),
      end(non_terminals.end());

  assert(non_terminals.size() != 0);
  for (; iter != end; ++iter) {
    IdentifierReferenceAdapter adapter(chip_, references_);
    adapter.DoEvaluate(parse_node);
  }
}

class LValueAdapter : public ParseNodeAdapter {
 public:
  LValueAdapter(CompositeChip const & chip) : chip_(chip) {}

  virtual void DoEvaluate(ParseNode const & parse_node);

  std::vector<IdentifierReference> const & references() const { return references_; }
 private:
  CompositeChip const & chip_;
  std::vector<IdentifierReference> references_;
};

void LValueAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(ParseNode::LVALUE == parse_node.type());
  /*
  if (context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
    if (!context.ConsumeNonTerminal(EvalIdentifierReferenceList))

    if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE))
    return context.Release();
  }

  if (!context.ConsumeNonTerminal(EvalIdentifierReference))
  */

  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();
  assert(non_terminals.size() == 1);

  std::vector<IdentifierReference> references;
  switch (non_terminals[0].first->type()) {
    case ParseNode::IDENTIFIER_REFERENCE:
      {
        IdentifierReferenceAdapter adapter(chip_, references);
        adapter.DoEvaluate(parse_node);
      }
      break;
    case ParseNode::IDENTIFIER_REFERENCE_LIST:
      {
        IdentifierReferenceListAdapter adapter(chip_, references);
        adapter.DoEvaluate(parse_node);
      }
      break;
    default:
      assert(false);
      break;
  }

  // Validate that the refences are good.
  std::vector<IdentifierReference>::const_iterator iter(references.begin()),
      end(references.end());
  for (; iter != end; ++iter) {
    if (iter->chip) {
      // TODO:  Add all input ports
      Chip* chip = iter->chip;
      std::vector<InputPin> & input_pins = chip->input_pins();
      std::vector<InputPin>::iterator pin_iter(input_pins.begin()),
        pin_end(input_pins.end());
      for (; pin_iter != pin_end; ++pin_iter) {
        IdentifierReference reference;
        reference.pin = &(*pin_iter);
        references_.push_back(reference);
      }
    }

    if (iter->pin) {
      if (!iter->pin->AsInput()) {
        // Output pin as an LValue - error
        assert(false);
      }
      IdentifierReference reference;
      reference.pin = iter->pin;
      references_.push_back(reference);
    }
  }
}

class ChipIdentifierReferenceListAdapter : public ParseNodeAdapter {
 public:
  ChipIdentifierReferenceListAdapter(WorkBench const & work_bench,
                                     CompositeChip& chip,
                                     ProgrammableChipBuilder& chip_builder,
                                     std::vector<IdentifierReference>& references)
    : work_bench_(work_bench),
      chip_(chip),
      chip_builder_(chip_builder),
      references_(references) {}

  virtual void DoEvaluate(ParseNode const & parse_node);
 private:
  WorkBench const & work_bench_;
  CompositeChip& chip_;
  ProgrammableChipBuilder& chip_builder_;

  std::vector<IdentifierReference>& references_;
};

class ChipIdentifierReferenceAdapter : public ParseNodeAdapter {
 public:
  ChipIdentifierReferenceAdapter(WorkBench const & work_bench,
                                 CompositeChip& chip,
                                 ProgrammableChipBuilder& chip_builder,
                                 std::vector<IdentifierReference>& references)
    : work_bench_(work_bench),
      chip_(chip),
      chip_builder_(chip_builder),
      references_(references) {}

  virtual void DoEvaluate(ParseNode const & parse_node);
 private:
  WorkBench const & work_bench_;
  CompositeChip& chip_;
  ProgrammableChipBuilder& chip_builder_;

  std::vector<IdentifierReference>& references_;
};

void ChipIdentifierReferenceListAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(ParseNode::CHIP_REFERENCE_LIST == parse_node.type());

  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();
  //ParseNode::TerminalArray const & terminals = parse_node.terminals();

  ParseNode::NonTerminalArray::const_iterator iter(non_terminals.begin()),
      end(non_terminals.end());
  for (; iter != end; ++iter) {
    switch (iter->first->type()) {
      case ParseNode::CHIP_REFERENCE:
        {
          ChipIdentifierReferenceAdapter adapter(work_bench_, chip_, chip_builder_,
                                                 references_);
          adapter.DoEvaluate(parse_node);
        }
        break;
      case ParseNode::IDENTIFIER_REFERENCE:
        {
          IdentifierReferenceAdapter adapter(chip_, references_);
          adapter.DoEvaluate(parse_node);
        }
        break;
      case ParseNode::IMMEDIATE_VALUE:
        {
          IdentifierReference reference;
          reference.immediate = iter->first->terminals()[0].first.type();
          references_.push_back(reference);
        }
        break;
      default:
        assert(false);
        break;
    }
  }
}

void ChipIdentifierReferenceAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(ParseNode::CHIP_REFERENCE == parse_node.type());
  /*
  if (!context.ConsumeToken(Token::IDENTIFIER))
  if (!context.ConsumeToken(Token::LEFT_PAREN))
  if (!context.ConsumeNonTerminal(EvalChipIdentifierReferenceList))
  if (!context.ConsumeToken(Token::RIGHT_PAREN))
  */

  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();
  ParseNode::TerminalArray const & terminals = parse_node.terminals();

  std::string const & chip_type = terminals[0].first.value();

  Board & board = chip_.board();

  ChipBuilder * builder = work_bench_.builder(chip_type);
  if (!builder) {
    // Use of undefined chip
    assert(false);
  }

  // TODO - Add chip with anonymous name
  Chip* new_chip = board.AddChip(builder->CreateInstance());
  chip_builder_.PushInstruction(new AddChipInstruction("", chip_type));

  std::vector<IdentifierReference> argument_references;
  ChipIdentifierReferenceListAdapter adapter(work_bench_, chip_, chip_builder_,
                                             argument_references);
  adapter.DoEvaluate(*non_terminals[0].first);

  std::vector<IdentifierReference>::const_iterator iter(argument_references.begin()),
      end(argument_references.end());
  int input_pin_offset = 0;
  for (; iter != end; ++iter) {
    if (iter->chip) {
      Chip* chip = iter->chip;

      std::vector<OutputPin> & output_pins = chip->output_pins();
      std::vector<OutputPin>::iterator pin_iter(output_pins.begin()),
          pin_end(output_pins.end());
      for (; pin_iter != pin_end; ++pin_iter) {
        // TODO
        // Create a new wire - unique name required
        Wire* wire = board.AddWire(new Wire(""));
        pin_iter->addWire(wire);
        new_chip->input_pins()[input_pin_offset].setWire(wire);

        // TODO:  Clean up the description interface - names are duplicated frequently
        chip_builder_.PushInstruction(new AddWireInstruction(wire->name()));
        chip_builder_.PushInstruction(new ConnectWireInstruction(
          wire->name(),
          chip->name(),
          pin_iter->name(),
          new_chip->name(),
          new_chip->description().input_port_name(input_pin_offset)));

        ++input_pin_offset;
      }
      // Connect all output pins to the corresponding indices on the new chip
      continue;
    }

    if (iter->wire) {
      Wire* wire = iter->wire;
      // Connect the wire to the new chip
      chip_builder_.PushInstruction(new ConnectWireToInputInstruction(
          wire->name(),
          new_chip->name(),
          new_chip->description().input_port_name(input_pin_offset)));;

      new_chip->input_pins()[input_pin_offset].setWire(wire);
      ++input_pin_offset;
      continue;
    }

    if (iter->pin) {
      OutputPin* output_pin = iter->pin->AsOutput();

      if (!output_pin) {
        // referencing non-output pin as output
        assert(false);
      }

      // Create an unnamed wire
      // Connect the wire from the pin to the input on the new chip
      Wire* wire = board.AddWire(new Wire(""));
      new_chip->input_pins()[input_pin_offset].setWire(wire);
      output_pin->addWire(wire);

      ++input_pin_offset;
      continue;
    }

    if (iter->immediate) {
      // Create an unnamed wire
      // Connect from the default instance for the immediate to the chip
      assert(false);
      // TODO:  Implement!
      ++input_pin_offset;
      continue;
    }

    assert(false);
  }
}

class RValueAdapter : public ParseNodeAdapter {
 public:
  RValueAdapter(WorkBench const & work_bench,
                CompositeChip& chip,
                ProgrammableChipBuilder& chip_builder)
    : work_bench_(work_bench),
      chip_(chip),
      chip_builder_(chip_builder) {}

  virtual void DoEvaluate(ParseNode const & parse_node);

  std::vector<IdentifierReference> const & references() const { return references_; }
 private:
  WorkBench const & work_bench_;
  CompositeChip& chip_;
  ProgrammableChipBuilder& chip_builder_;
  std::vector<IdentifierReference> references_;
};

void RValueAdapter::DoEvaluate(ParseNode const & parse_node) {
  /*
  if (context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
    if (!context.ConsumeNonTerminal(EvalChipIdentifierReferenceList))

    if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE))

    context.node()->set_type(ParseNode::RVALUE_LIST);
    return context.Release();
  }

  if (!context.ConsumeNonTerminal(EvalChipIdentifierReference))
  */

  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();
  assert(non_terminals.size() == 1);

  switch (non_terminals[0].first->type()) {
    case ParseNode::CHIP_REFERENCE:
      {
        std::vector<IdentifierReference> references;
        IdentifierReferenceAdapter adapter(chip_, references);
        adapter.DoEvaluate(parse_node);
      }
      break;
    case ParseNode::CHIP_REFERENCE_LIST:
      {
//        IdentifierReferenceListAdapter adapter(chip_, references_);
//        adapter.DoEvaluate(parse_node);
        assert(false);
      }
      break;
    default:
      assert(false);
      break;
  }
}

class LeftAssignStatementAdapter : public ParseNodeAdapter {
 public:
  LeftAssignStatementAdapter(WorkBench const & work_bench,
                             CompositeChip& chip,
                             ProgrammableChipBuilder& chip_builder)
    : work_bench_(work_bench),
      chip_(chip),
      chip_builder_(chip_builder) {}

  virtual void DoEvaluate(ParseNode const & parse_node);
 private:
  WorkBench const & work_bench_;
  CompositeChip& chip_;
  ProgrammableChipBuilder& chip_builder_;
};

void LeftAssignStatementAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(ParseNode::LEFT_ASSIGN_STATEMENT == parse_node.type());

  /*
  if (!context.ConsumeNonTerminal(EvalLValue))
  if (!context.ConsumeToken(Token::LEFT_ARROW))
  if (!context.ConsumeNonTerminal(EvalRValue))
  if (!context.ConsumeToken(Token::SEMI_COLON))
  */

  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();

  LValueAdapter l_adapter(chip_);
  l_adapter.DoEvaluate(*non_terminals[0].first);

  RValueAdapter r_adapter(work_bench_, chip_, chip_builder_);
  r_adapter.DoEvaluate(*non_terminals[1].first);

  std::vector<IdentifierReference> const & l_references = l_adapter.references();
  std::vector<IdentifierReference> const & r_references = r_adapter.references();

  {
    std::vector<IdentifierReference>::const_iterator iter(l_references.begin()),
        end(l_references.end());
    for (; iter != end; ++iter) {
      assert(iter->pin != NULL);
    }

    iter = r_references.begin();
    end = r_references.end();
    for (; iter != end; ++iter) {
      assert(iter->pin != NULL);
    }
  }

  if (l_references.size() != r_references.size()) {
    // log error:  Incompatible assignment size
    assert(false);
  }

  std::vector<IdentifierReference>::const_iterator l_iter(l_references.begin()),
      l_end(l_references.end()),
      r_iter(r_references.begin()),
      r_end(r_references.end());
  for (; l_iter != l_end; ++l_iter, ++r_iter) {
    assert(r_iter != r_end);

    // TODO: Anonymous names
    chip_builder_.PushInstruction( new AddWireInstruction("") );
    chip_builder_.PushInstruction( new ConnectWireInstruction("", "", "", "", ""));
  }
}

class ChipBodyAdapter : public ParseNodeAdapter {
 public:
  ChipBodyAdapter(WorkBench const & work_bench,
                  ProgrammableChipBuilder& builder);

  virtual void DoEvaluate(ParseNode const & parse_node);
 private:
  WorkBench const & work_bench_;
  ProgrammableChipBuilder& chip_builder_;
  CompositeChip composite_chip_;  // Local symbol table for composite chip
};

ChipBodyAdapter::ChipBodyAdapter(WorkBench const & work_bench,
                                 ProgrammableChipBuilder& builder)
  : work_bench_(work_bench),
    chip_builder_(builder),
    composite_chip_(*builder.description()) {
}

void ChipBodyAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(ParseNode::CHIP_BODY == parse_node.type());

  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();
  ParseNode::NonTerminalArray::const_iterator iter(non_terminals.begin()),
      end(non_terminals.end());

  for (; iter != end; ++iter) {
    switch (iter->first->type()) {
      case ParseNode::CHIP_INSTANCE:
        {
          ChipInstanceAdapter adapter(work_bench_, composite_chip_, chip_builder_);
          adapter.DoEvaluate(*iter->first);
        }
        break;
      case ParseNode::WIRE_INSTANCE:
        {
          WireInstanceAdapter adapter(work_bench_, composite_chip_, chip_builder_);
          adapter.DoEvaluate(*iter->first);
        }
        break;
      case ParseNode::LEFT_ASSIGN_STATEMENT:
        {
          LeftAssignStatementAdapter adapter(work_bench_, composite_chip_, chip_builder_);
          adapter.DoEvaluate(*iter->first);
        }
        break;
      case ParseNode::RIGHT_ASSIGN_STATEMENT:
        {
          assert(false);
        }
        break;
      default:
        assert(false);
        break;
    }
  }
}

#include <utility>

class ChipDefinitionAdapter : public ParseNodeAdapter {
 public:
  ChipDefinitionAdapter(WorkBench const & work_bench,
                        ProgrammableChipBuilder& chip_builder);
  virtual void DoEvaluate(ParseNode const & parse_node);

  //ChipDescription const * description() const { return description_; }
 private:
  WorkBench const & work_bench_;
  std::auto_ptr<CompositeChip> composite_chip_;  // Used as the symbol table for this chip
  ProgrammableChipBuilder& chip_builder_;
};

ChipDefinitionAdapter::ChipDefinitionAdapter(WorkBench const & work_bench,
                                             ProgrammableChipBuilder & chip_builder)
  : work_bench_(work_bench),
    chip_builder_(chip_builder) {
}

void ChipDefinitionAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(parse_node.type() == ParseNode::CHIP_DEFINITION);

  ParseNode::TerminalArray const & terminals = parse_node.terminals();
  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();

  std::string chip_name = terminals[1].first.value();

  IdentifierDefinitionListAdapter input_pin_definitions;
  IdentifierDefinitionListAdapter output_pin_definitions;

  input_pin_definitions.DoEvaluate(*non_terminals[0].first);
  output_pin_definitions.DoEvaluate(*non_terminals[1].first);

  // Build the ChipDescription instance
  std::vector<std::string> input_pins;
  std::vector<std::string> output_pins;

  std::vector<IdentifierDefinition> const & output_definitions = output_pin_definitions.definition_list();
  std::vector<IdentifierDefinition>::const_iterator output_iter(output_definitions.begin()),
      output_end(output_definitions.end());
  for (; output_iter != output_end; ++output_iter) {
    AppendNamesFromDefinition(*output_iter, &output_pins);
  }

  std::vector<IdentifierDefinition> const & input_definitions = input_pin_definitions.definition_list();
  std::vector<IdentifierDefinition>::const_iterator iter(input_definitions.begin()),
      end(input_definitions.end());
  for (; iter != end; ++iter) {
    AppendNamesFromDefinition(*iter, &input_pins);
  }

  ChipDescription * chip_description = ChipDescription::Create(chip_name, input_pins, output_pins);
  chip_builder_.set_description(chip_description);
  composite_chip_.reset(new CompositeChip(*chip_description));

  /*
  if (!context.ConsumeToken(Token::CHIP))
  if (!context.ConsumeToken(Token::IDENTIFIER))
  if (!context.ConsumeToken(Token::LEFT_PAREN))
  if (!context.ConsumeNonTerminal(EvalIdentifierList))
  if (!context.ConsumeToken(Token::RIGHT_PAREN))
  if (!context.ConsumeToken(Token::RIGHT_ARROW))
  if (!context.ConsumeToken(Token::LEFT_PAREN))
  if (!context.ConsumeNonTerminal(EvalIdentifierList))
  if (!context.ConsumeToken(Token::RIGHT_PAREN))
  if (!context.ConsumeToken(Token::LEFT_BRACE))
  if (!context.ConsumeNonTerminal(EvalChipBody))
  if (!context.ConsumeToken(Token::RIGHT_BRACE))
  */

  ChipBodyAdapter body_adapter(work_bench_, chip_builder_);
  body_adapter.DoEvaluate(*non_terminals[2].first);

  // Construct the chip body
}

class SemanticParseTreeVisitor : public ParseNode::Visitor {
 public:
  SemanticParseTreeVisitor() {}
  virtual ~SemanticParseTreeVisitor() {}

  virtual void VisitTerminal(Token const & token, size_t offset);
  virtual void VisitNonTerminal(ParseNode const * parse_node,
                                size_t offset);

 private:
  DISALLOW_COPY_AND_ASSIGN(SemanticParseTreeVisitor);
};

void SemanticParseTreeVisitor::VisitTerminal(Token const & token,
                                             size_t offset) {
}

#include "chip/thd_work_bench.h"

void SemanticParseTreeVisitor::VisitNonTerminal(ParseNode const * parse_node,
                                                size_t offset) {
  SemanticParseTreeVisitor output_visitor;
  parse_node->VisitChildrenLeftToRight(&output_visitor);

  if (parse_node->type() == ParseNode::CHIP_DEFINITION) {
    ThdWorkBench bench;
    ProgrammableChipBuilder builder(bench);
    ChipDefinitionAdapter adapter(bench, builder);
    adapter.DoEvaluate(*parse_node);
  }
}

int main(void) {
  std::ifstream fstream("..\\parser\\example.thd");

  if (!fstream) {
    return 0;
  }

  IoStream io_stream(&fstream);

  LineCountingStream line_count(io_stream);
  size_t current_count = line_count.GetCount();

  TokenStream token_stream(line_count);

  BufferedTokenStream buffered_stream(token_stream);

  //ParseNode* chip_node = new ParseNode;
  //ParseErrorCollection errors;
  Parser parser(buffered_stream);

  parser.Evaluate();
  ParseNode* chip_node = parser.node();

  //ParseNode* chip_node = EvalTHDFile(buffered_stream, &errors);

  std::cout << *chip_node;

  SemanticParseTreeVisitor visitor;
  chip_node->VisitChildrenLeftToRight(&visitor);
}
