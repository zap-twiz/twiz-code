
#include "parser/parse_node.h"

#include <assert.h>

namespace {

class IndentedStream {
 public:
  explicit IndentedStream(std::ostream& stream) : stream_(stream), indent_(0) {}
  void AddIndent(int indent) { indent_ += indent; }

  void Indent() {
    for (int x = 0; x < indent_; ++x) {
      stream_ << '\t';
    }
  }

  std::ostream& out() { return stream_; }
  operator std::ostream&() { return stream_; }

 private:
  int indent_;
  std::ostream& stream_;
};

std::ostream& operator<<(IndentedStream& stream, ParseNode const & node);
class StreamVisitor: public ParseNode::Visitor {
 public:
  StreamVisitor(IndentedStream& stream) : stream_(stream) {}

  virtual void VisitTerminal(Token const & token, size_t offset) {
    stream_.Indent();
    stream_.out() << "<TOKEN ";
    stream_.out() << "Type=\"" <<Token::kTokenTypeNames[token.type()] << "\" ";
    stream_.out() << "Value=\"" << token.value() << "\" ";
    stream_.out() << "Line=\"" << token.line() << "\" />" << std::endl;
  }

  virtual void VisitNonTerminal(ParseNode const * parse_node,
                                size_t offset) {
    operator<<(stream_, *parse_node);
  }
 private:
  IndentedStream& stream_;
};

std::ostream& operator<<(IndentedStream& stream, ParseNode const & node) {
  stream.Indent();
  static_cast<std::ostream&>(stream) << "<" << ParseNode::kProductionTypeNames[node.type()]
      << ">" << std::endl;
  stream.AddIndent(1);

  StreamVisitor output_visitor(stream);
  node.VisitChildrenLeftToRight(&output_visitor);

  stream.AddIndent(-1);
  stream.Indent();

  static_cast<std::ostream&>(stream) << "</" << ParseNode::kProductionTypeNames[node.type()]
      << ">" << std::endl;

  return stream;
}

}  // unnamed

char const * const ParseNode::kProductionTypeNames[] = {
    "THD_MODULE",
    "IDENTIFIER_DEFINITION",
    "IDENTIFIER_DEFINITION_LIST",
    "IDENTIFIER_REFERENCE",
    "IDENTIFIER_REFERENCE_LIST",
    "CHIP_REFERENCE",
    "CHIP_REFERENCE_LIST",
    "CHIP_INSTANCE",
    "WIRE_INSTANCE",
    "LEFT_ASSIGN_STATEMENT",
    "RIGHT_ASSIGN_STATEMENT",
    "CHIP_BODY",
    "LVALUE_LIST",
    "LVALUE",
    "RVALUE_LIST",
    "RVALUE",
    "IMMEDIATE_VALUE",
    "CHIP_DEFINITION",
    "SINGLE_PIN_DEFINITION",
    "IMPORT_STATEMENT",

    "NUMBER_RANGE",
    "NUMBER_COLLECTION",
    "NUMBER",
    "UNKNOWN"
  };


void ParseNode::FreeChildren() {
  NonTerminalArray::iterator iter(non_terminals_.begin()),
    end(non_terminals_.end());
  for (; iter != end; ++iter) {
    delete iter->first;
  }

  non_terminals_.clear();
  terminals_.clear();

  type_ = UNKNOWN;
}

void ParseNode::VisitChildrenRightToLeft(Visitor * visitor) const {
  if (!visitor)
    return;

  if (terminals_.empty() && non_terminals_.empty())
    return;

  TerminalArray::const_reverse_iterator terminal_iter(terminals_.rbegin()),
      terminal_end(terminals_.rend());

  NonTerminalArray::const_reverse_iterator
      non_terminal_iter(non_terminals_.rbegin()),
      non_terminal_end(non_terminals_.rend());

  size_t offset = size();
  for (; offset != 0; --offset) {
    if (terminal_end != terminal_iter
        && offset - 1 == terminal_iter->second) {
      visitor->VisitTerminal(terminal_iter->first, offset - 1);
      ++terminal_iter;
      continue;
    }

    if (non_terminal_end != non_terminal_iter
        && offset - 1 == non_terminal_iter->second) {
      visitor->VisitNonTerminal(non_terminal_iter->first, offset - 1);
      ++non_terminal_iter;
      continue;
    }

    assert(false);
  }
}

void ParseNode::VisitChildrenLeftToRight(Visitor * visitor) const {
  if (!visitor)
    return;

  if (terminals_.empty() && non_terminals_.empty())
    return;

  TerminalArray::const_iterator terminal_iter(terminals_.begin()),
      terminal_end(terminals_.end());

  NonTerminalArray::const_iterator
      non_terminal_iter(non_terminals_.begin()),
      non_terminal_end(non_terminals_.end());

  size_t offset = 0;
  for (; offset < size(); ++offset) {
    if (terminal_end != terminal_iter
        && offset == terminal_iter->second) {
      visitor->VisitTerminal(terminal_iter->first, offset);
      ++terminal_iter;
      continue;
    }

    if (non_terminal_end != non_terminal_iter
        && offset == non_terminal_iter->second) {
      visitor->VisitNonTerminal(non_terminal_iter->first, offset);
      ++non_terminal_iter;
      continue;
    }

    assert(false);
  }
}

std::ostream& operator<<(std::ostream& stream, ParseNode const & node) {
  IndentedStream indented(stream);

  indented.out() << "<" << ParseNode::kProductionTypeNames[node.type()] << ">" << std::endl;
  indented.AddIndent(1);

  StreamVisitor output_visitor(indented);
  node.VisitChildrenLeftToRight(&output_visitor);

  indented.AddIndent(-1);
  indented.out() << "</" << ParseNode::kProductionTypeNames[node.type()] << ">";
  return stream;
}
