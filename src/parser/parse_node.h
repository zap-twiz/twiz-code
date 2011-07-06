#ifndef INCLUDED_PARSER_PARSE_NODE_H_
#define INCLUDED_PARSER_PARSE_NODE_H_

#include "base/base.h"
#include "lexer/token.h"

#include <vector>
#include <utility>


class ParseNode {
 public:
  enum ProductionType {
    CHIP_DESCRIPTION,
    ARGUMENT_DEFINITION,
    PIN_DEFINITION,
    WIRE_DEFINITION,
    PIN_ASSIGNMENT,
    PIN_REFERENCE,
    PIN_COLLECTION,
    CHIP_INSTANCE,
    CHIP_REFERENCE,
    UNKNOWN
  };

  ParseNode() : type_(UNKNOWN) {}
  ~ParseNode() {} // TODO:  Fix leaking parse node children

  ProductionType type() const { return type_; }
  void set_type(ProductionType type) { type_ = type; }

  void PushTerminal(Token const & token) {
    terminals_.push_back(
        std::make_pair(token, size()));
  }

  void PushNonTerminal(ParseNode * parse_node) {
    non_terminals_.push_back(
        std::make_pair(parse_node, size()));
  }

  size_t size() const { return terminals_.size() + non_terminals_.size(); }

  class Visitor {
   public:
    virtual ~Visitor() {}
    virtual void VisitTerminal(Token const & token, size_t offset) = 0;
    virtual void VisitNonTerminal(ParseNode const * parse_node,
                                  size_t offset) = 0;
  };
  void VisitChildrenRightToLeft(Visitor * visitor) const;
  void VisitChildrenLeftToRight(Visitor * visitor) const;

#if 0
  std::vector<Token>& terminals() { return terminals_; }
  std::vector<ParseNode>& non_terminals() { return non_terminal_children_; }
#endif

 private:
  ProductionType type_;

  typedef std::pair<ParseNode*, int> NonTerminalReference;
  typedef std::pair<Token, int> TerminalReference;

  typedef std::vector<NonTerminalReference> NonTerminalArray;
  typedef std::vector<TerminalReference> TerminalArray;

  NonTerminalArray non_terminals_;
  TerminalArray terminals_;

  // No copy or assign!
  DISALLOW_COPY_AND_ASSIGN(ParseNode);
};

#endif  //INCLUDED_PARSER_PARSE_NODE_H_
