
#include "parser/parse_error_collection.h"
#include "parser/parser.h"

#include "base/string_utils.h"
#include "lexer/line_counting_stream.h"
#include "lexer/token_stream.h"
#include "streams/iostream.h"

#include "chip/chip_description.h"


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

class ThdModuleVisitor : public ParseNode::Visitor {
};

class IdentifierDefinitionVisitor : public ParseNode::Visitor {
};

class IdentifierDefinitionListVisitor : public ParseNode::Visitor {
};

class IdentifierReferenceVisitor : public ParseNode::Visitor {
};

class IdentifierReferenceListVisitor : public ParseNode::Visitor {
};

class ChipReferenceVisitor : public ParseNode::Visitor {
};

class ChipReferenceListVisitor : public ParseNode::Visitor {
};

class ChipInstanceVisitor : public ParseNode::Visitor {
};

class WireInstanceVisitor : public ParseNode::Visitor {
};

class LeftAssignStatementVisitor : public ParseNode::Visitor {
};

class RightAssignStatementVisitor : public ParseNode::Visitor {
};

class ChipBodyVisitor : public ParseNode::Visitor {
};

class LValueListVisitor : public ParseNode::Visitor {
};

class LValueVisitor : public ParseNode::Visitor {
};

class RValueListVisitor : public ParseNode::Visitor {
};

class RValueVisitor : public ParseNode::Visitor {
};

class ImmediateValueVisitor : public ParseNode::Visitor {
};

class ChipDefinitionVisitor : public ParseNode::Visitor {
};

class SinglePinDefinition : public ParseNode::Visitor {
};

class ImportStatementVisitor : public ParseNode::Visitor {
};

class NumberRangeVisitor : public ParseNode::Visitor {
};

class NumberCollectionVisitor : public ParseNode::Visitor {
};

class NumberVisitor : public ParseNode::Visitor {
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
#include "chip/work_bench.h"

class ChipInstanceAdapter : public ParseNodeAdapter {
 public:
  ChipInstanceAdapter(WorkBench const & work_bench,
                      CompositeChip& chip)
    : work_bench_(work_bench),
      chip_(chip) {}

  virtual void DoEvaluate(ParseNode const & parse_node);
 private:
  WorkBench const & work_bench_;
  CompositeChip& chip_;
};

void ChipInstanceAdapter::DoEvaluate(ParseNode const & parse_node) {
  /*
  if (!context.ConsumeToken(Token::CHIP))
  if (!context.ConsumeToken(Token::IDENTIFIER))
  if (!context.ConsumeNonTerminal(EvalIdentifierList))
  if (!context.ConsumeToken(Token::SEMI_COLON))
  */
  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();

  std::string const chip_type = parse_node.terminals()[1].first.value();

  ChipBuilder* builder = work_bench_.builder(chip_type);
  if (!builder) {
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
        // TODO:  Log semantic error
      }
      Chip* new_chip = builder->CreateInstance();
      new_chip->set_name(*name_iter);
      board.AddChip(new_chip);
    }
  }
}

class ChipBodyAdapter : public ParseNodeAdapter {
 public:
  ChipBodyAdapter(ChipDescription const & chip_description,
                  WorkBench const & work_bench);

  virtual void DoEvaluate(ParseNode const & parse_node);
 private:
  ChipDescription const & description_;
  WorkBench const & work_bench_;
  CompositeChip composite_chip_;
};

ChipBodyAdapter::ChipBodyAdapter(ChipDescription const & chip_description,
                                 WorkBench const & work_bench)
  : description_(chip_description),
    work_bench_(work_bench),
    composite_chip_(chip_description) {
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
          ChipInstanceAdapter adapter(work_bench_, composite_chip_);
          adapter.DoEvaluate(*iter->first);
        }
        break;
      case ParseNode::WIRE_INSTANCE:
        break;
      case ParseNode::LEFT_ASSIGN_STATEMENT:
        break;
      case ParseNode::RIGHT_ASSIGN_STATEMENT:
        break;
      default:
        assert(false);
        break;
    }
  }

#if 0
   static NonTerminalEvaluator chip_body_statements[] = {
      EvalChipInstance,
      EvalWireInstance,
      EvalLeftAssignStatement,
      EvalRightAssignStatement,
      NULL
    };

  while (context.ConsumeNonTerminal(chip_body_statements)) {
  }
#endif

  //context.node()->set_type(ParseNode::CHIP_BODY);


}

class ChipDefinitionAdapter : public ParseNodeAdapter {
 public:
  virtual void DoEvaluate(ParseNode const & parse_node);

  ChipDescription const * description() const { return description_; }
 private:
  ChipDescription* description_;
};

void ChipDefinitionAdapter::DoEvaluate(ParseNode const & parse_node) {
  assert(parse_node.type() == ParseNode::CHIP_DEFINITION);

  ParseNode::TerminalArray const & terminals = parse_node.terminals();
  ParseNode::NonTerminalArray const & non_terminals = parse_node.non_terminals();

  std::string chip_name = terminals[1].first.value();

  IdentifierDefinitionListAdapter input_pin_definitions;
  IdentifierDefinitionAdapter output_pin_definition;

  input_pin_definitions.DoEvaluate(*non_terminals[0].first);
  output_pin_definition.DoEvaluate(*non_terminals[1].first);

  // Build the ChipDescription instance
  std::vector<std::string> input_pins;
  std::vector<std::string> output_pins;

  AppendNamesFromDefinition(output_pin_definition.definition(), &output_pins);

  std::vector<IdentifierDefinition> const & input_definitions = input_pin_definitions.definition_list();
  std::vector<IdentifierDefinition>::const_iterator iter(input_definitions.begin()),
      end(input_definitions.end());
  for (; iter != end; ++iter) {
    AppendNamesFromDefinition(*iter, &input_pins);
  }

  description_ = ChipDescription::Create(chip_name, input_pins, output_pins);
  /*
  if (!context.ConsumeToken(Token::CHIP))
  if (!context.ConsumeToken(Token::IDENTIFIER))
  if (!context.ConsumeToken(Token::LEFT_PAREN))
  if (!context.ConsumeNonTerminal(EvalIdentifierList))
  if (!context.ConsumeToken(Token::RIGHT_PAREN))
  if (!context.ConsumeToken(Token::RIGHT_ARROW))
  if (!context.ConsumeNonTerminal(EvalIdentifierDefinition))
  if (!context.ConsumeToken(Token::LEFT_BRACE))
  if (!context.ConsumeNonTerminal(EvalChipBody))
  if (!context.ConsumeToken(Token::RIGHT_BRACE))
  */

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

void SemanticParseTreeVisitor::VisitNonTerminal(ParseNode const * parse_node,
                                                size_t offset) {
  SemanticParseTreeVisitor output_visitor;
  parse_node->VisitChildrenLeftToRight(&output_visitor);

  if (parse_node->type() == ParseNode::CHIP_DEFINITION) {
    ChipDefinitionAdapter adapter;
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

#if 0
std::vector<int> NumberRangeAdapter(ParseNode const * parse_node) {
  int range_min = TokenToInt(parse_node->terminals()[0].first);
  int range_max = TokenToInt(parse_node->terminals()[2].first);

  std::vector<int> range;
  for (int x = range_min; x <= range_max; ++x) {
    range.push_back(x);
  }
  return range;
}

std::vector<int> NumberCollectionAdapter(ParseNode const * parse_node) {
  return std::vector<int>();
}

std::vector<int> GetNumberRange(ParseNode const * parse_node) {
  std::vector<int> numbers;
  switch(parse_node->type()) {
    case ParseNode::NUMBER_RANGE:
      break;
    case ParseNode::NUMBER_COLLECTION:
      break;
    case ParseNode::NUMBER:
      numbers.push_back(TokenToInt(parse_node->terminals()[0].first));
      break ;
    default:
      assert(false);
      break;
  }
  return numbers;
}
#endif
