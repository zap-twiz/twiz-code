
#include "parser/parse_error_collection.h"
#include "parser/parser.h"

#include "base/string_utils.h"
#include "lexer/line_counting_stream.h"
#include "lexer/token_stream.h"
#include "streams/iostream.h"

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

class IdentifierDefinition {
 public:
  std::string name;
  int array_size;
};

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

  if (parse_node->type() == ParseNode::NUMBER_RANGE) {
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
