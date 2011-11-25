#ifndef INCLUDED_PARSER_PARSE_NODE_H_
#define INCLUDED_PARSER_PARSE_NODE_H_

#include "base/base.h"
#include "lexer/token.h"

#include <vector>
#include <utility>

#include <iostream>

class ParseNode {
 public:
  enum ProductionType {
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

    // OLD DEFINITIONS
    ARGUMENT_DEFINITION,
    ARRAY_PIN_DEFINITION,
    WIRE_DECLARATION,
    PIN_ASSIGNMENT,
    PIN_REFERENCE,
    PIN_COLLECTION,
    CHIP_DECLARATION,
    NUMBER_RANGE,
    NUMBER_COLLECTION,
    NUMBER,
    UNKNOWN
  };

  typedef std::pair<ParseNode*, int> NonTerminalReference;
  typedef std::pair<Token, int> TerminalReference;

  typedef std::vector<NonTerminalReference> NonTerminalArray;
  typedef std::vector<TerminalReference> TerminalArray;

  ParseNode() : type_(UNKNOWN) {}
  ~ParseNode() {} // TODO:  Fix leaking parse node children

  ProductionType type() const { return type_; }
  void set_type(ProductionType type) { type_ = type; }

  void PushTerminal(Token const & token) {
    terminals_.push_back(
        std::make_pair(token, size()));
  }

  void PushNonTerminal(ParseNode * parse_node) {
    non_terminals_.push_back(std::make_pair(parse_node, size()));
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

  TerminalArray& terminals() { return terminals_; }
  NonTerminalArray& non_terminals() { return non_terminals_; }

 private:
  ProductionType type_;

  NonTerminalArray non_terminals_;
  TerminalArray terminals_;

  // No copy or assign!
  DISALLOW_COPY_AND_ASSIGN(ParseNode);
};

std::ostream& operator<<(std::ostream& stream, ParseNode const & node);
#endif  //INCLUDED_PARSER_PARSE_NODE_H_
