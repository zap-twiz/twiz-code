#ifndef INCLUDED_PARSER_PARSER_H_
#define INCLUDED_PARSER_PARSER_H_

#include "lexer/token.h"
#include "parser/parse_node.h"
#include "parser/parser_utils.h"

typedef BufferedStream<Token> BufferedTokenStream;

class ParserEntries {
 public:
  ParserEntries() : node_(NULL), error_collection_(NULL) {}
  ~ParserEntries() {
  }

  // Entry points - exposed for testing purposes
  bool ParseImmediateValue(BufferedTokenStream& input_stream);
  bool ParseIdentifierDefinition(BufferedTokenStream& input_stream);
  bool ParseNumberOrRange(BufferedTokenStream& input_stream);
  bool ParseNumberCollection(BufferedTokenStream& input_stream);
  bool ParseIdentifierReference(BufferedTokenStream& input_stream);
  bool ParseChipIdentifierReference(BufferedTokenStream& input_stream);
  bool ParseChipIdentifierReferenceList(BufferedTokenStream& input_stream);
  bool ParseIdentifierReferenceList(BufferedTokenStream& input_stream);
  bool ParseIdentifierList(BufferedTokenStream& input_stream);
  bool ParseChipInstance(BufferedTokenStream& input_stream);
  bool ParseWireInstance(BufferedTokenStream& input_stream);
  bool ParseRValue(BufferedTokenStream& input_stream);
  bool ParseLValue(BufferedTokenStream& input_stream);
  bool ParseLeftAssignStatement(BufferedTokenStream& input_stream);
  bool ParseRightAssignStatement(BufferedTokenStream& input_stream);
  bool ParseChipBody(BufferedTokenStream& input_stream);
  bool ParseChipDefinition(BufferedTokenStream& input_stream);
  bool ParseImportClause(BufferedTokenStream& input_stream);
  bool ParseTHDFile(BufferedTokenStream& input_stream);

  ParseNode* node() { return node_; }
  ParseErrorCollection* error_collection() { error_collection_; }
  void Release() {
    node_ = NULL;
    error_collection_ = NULL;
  }

 private:
  ParseNode* node_;
  ParseErrorCollection* error_collection_;

  void InitForParsing();

  DISALLOW_COPY_AND_ASSIGN(ParserEntries);
};

class Parser {
 public:
  Parser(BufferedTokenStream& input_stream);

  bool Evaluate();

  ParseNode* node() { return entries_.node(); }
  ParseErrorCollection* error_collection() { entries_.error_collection(); }

 private:
  ParserEntries entries_;
  BufferedTokenStream& input_stream_;

  DISALLOW_COPY_AND_ASSIGN(Parser);
};

#endif  //INCLUDED_PARSER_PARSER_H_
