
#include "parser/parser.h"

#include "base/base.h"
#include "lexer/token_stream.h"
#include "lexer/line_counting_stream.h"
#include "streams/iostream.h"
#include "streams/buffered_stream.h"

#include <fstream>

#include <iostream>
#include <memory>
#include <vector>

#include "parser/parse_error.h"
#include "parser/parse_error_collection.h"
#include "parser/parse_node.h"
#include "parser/parser_utils.h"

namespace {

class ParsingContext {
 public:
  ParsingContext(BufferedTokenStream& input_stream,
                 ParseErrorCollection* error_collection)
    : input_stream_(input_stream),
      error_collection_(error_collection),
      node_(new ParseNode),
      unwinder_(&input_stream_, node_) {
  }

  ~ParsingContext() {
    if (!node_) {
      unwinder_.Release();
    }

    unwinder_.Unwind();

    delete node_;
  }

  ParseNode* node() { return node_; }
  ParseNode* Release() { ParseNode* node = node_; node_ = NULL; return node; }

  bool ConsumeToken(Token::TokenType token_type) {
    return ::ConsumeToken(input_stream_, node_, token_type);
  }

  bool ConsumeToken(Token::TokenType* token_types) {
    return ::ConsumeToken(input_stream_, node_, token_types);
  }

  bool ConsumeNonTerminal(NonTerminalEvaluator evaluator) {
    return ::ConsumeNonTerminal(input_stream_, node_, error_collection_,
                                evaluator);
  }

  bool ConsumeNonTerminal(NonTerminalEvaluator* evaluators) {
    return ::ConsumeNonTerminal(input_stream_, node_, error_collection_,
                                evaluators);
  }

 private:
  BufferedTokenStream& input_stream_;
  ParseErrorCollection* error_collection_;
  ParseNode* node_;
  AutoParseNodeUnwinder unwinder_;
};

// HIGH
// LOW
// TRUE
// FALSE
ParseNode* EvalImmediateValue(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  static Token::TokenType immediate_tokens[] = {
      Token::HIGH,
      Token::LOW,
      Token::TRUE,
      Token::FALSE,
      Token::UNKNOWN
    };

  if (!context.ConsumeToken(immediate_tokens))
    return NULL;

  context.node()->set_type(ParseNode::IMMEDIATE_VALUE);
  return context.Release();
}

// Identifier Definition
// name
// name[NUMBER]
ParseNode* EvalIdentifierDefinition(BufferedTokenStream& input_stream,
                                    ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);
  if (!context.ConsumeToken(Token::IDENTIFIER))
    return NULL;

  if (!context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
    context.node()->set_type(ParseNode::SINGLE_PIN_DEFINITION);
    return context.Release();
  }

  static Token::TokenType kNumberTokens[] = {
    Token::NUMBER_BINARY,
    Token::NUMBER_DECIMAL,
    Token::NUMBER_HEX,
    static_cast<Token::TokenType>(0)
  };

  if (!context.ConsumeToken(kNumberTokens)) {
    return NULL;
  }

  if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE)) {
    return NULL;
  }

  context.node()->set_type(ParseNode::IDENTIFIER_DEFINITION);
  return context.Release();
}

// #
// #..#
ParseNode* EvalNumberOrRange(BufferedTokenStream& input_stream,
                             ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);
  static Token::TokenType kNumberTokens[] = {
    Token::NUMBER_BINARY,
    Token::NUMBER_DECIMAL,
    Token::NUMBER_HEX,
    static_cast<Token::TokenType>(0)
  };

  if (!context.ConsumeToken(kNumberTokens))
    return NULL;

  // A range is specified
  if (!context.ConsumeToken(Token::DOT_DOT)) {
    context.node()->set_type(ParseNode::NUMBER);
    return context.Release();
  }

  if (!context.ConsumeToken(kNumberTokens))
    return NULL;

  context.node()->set_type(ParseNode::NUMBER_RANGE);
  return context.Release();
}

// #
// #, #, #
// #..#, #, #..#
ParseNode* EvalNumberList(BufferedTokenStream& input_stream,
                          ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeNonTerminal(EvalNumberOrRange))
    return NULL;

  while(context.ConsumeToken(Token::COMMA)) {
    if (!context.ConsumeNonTerminal(EvalNumberOrRange))
      return NULL;
  }

  context.node()->set_type(ParseNode::NUMBER_COLLECTION);
  return context.Release();
}

// name
// name[number_list]
// name.pin
// name[number_list].pin[number_list]
ParseNode* EvalIdentifierReference(BufferedTokenStream& input_stream,
                                   ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeToken(Token::IDENTIFIER))
    return NULL;

  if (context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
    if (!context.ConsumeNonTerminal(EvalNumberList))
      return NULL;

    if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE))
      return NULL;
  }

  if (context.ConsumeToken(Token::DOT)) {
    if (!context.ConsumeToken(Token::IDENTIFIER))
      return NULL;

    if (context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
      if (!context.ConsumeNonTerminal(EvalNumberList))
        return NULL;

      if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE))
        return NULL;
    }
  }

  context.node()->set_type(ParseNode::IDENTIFIER_REFERENCE);
  return context.Release();
}

ParseNode* EvalChipIdentifierReferenceList(
    BufferedTokenStream& input_stream,
    ParseErrorCollection* error_collection);

// Evaluates a chip instance in an R-Value
ParseNode* EvalChipIdentifierReference(BufferedTokenStream& input_stream,
                                       ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeToken(Token::IDENTIFIER))
    return NULL;

  if (!context.ConsumeToken(Token::LEFT_PAREN))
    return NULL;

  if (!context.ConsumeNonTerminal(EvalChipIdentifierReferenceList))
    return NULL;

  if (!context.ConsumeToken(Token::RIGHT_PAREN))
    return NULL;

  context.node()->set_type(ParseNode::CHIP_REFERENCE);
  return context.Release();
}

ParseNode* EvalChipIdentifierReferenceList(
    BufferedTokenStream& input_stream,
    ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  static NonTerminalEvaluator chip_or_identifier[] = {
      EvalChipIdentifierReference,
      EvalIdentifierReference,
      EvalImmediateValue,
      NULL
    };

  if (!context.ConsumeNonTerminal(chip_or_identifier))
    return NULL;

  while (context.ConsumeToken(Token::COMMA)) {
    if (!context.ConsumeNonTerminal(chip_or_identifier))
      return NULL;
  }

  context.node()->set_type(ParseNode::CHIP_REFERENCE_LIST);
  return context.Release();
}

// identifier-reference
// identifier-reference, identifier-reference
ParseNode* EvalIdentifierReferenceList(BufferedTokenStream& input_stream,
                                       ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeNonTerminal(EvalIdentifierReference))
    return NULL;

  while (context.ConsumeToken(Token::COMMA)) {
    if (!context.ConsumeNonTerminal(EvalIdentifierReference))
      return NULL;
  }

  context.node()->set_type(ParseNode::IDENTIFIER_REFERENCE_LIST);
  return context.Release();
}

// identifier
// identifier , identifier
ParseNode* EvalIdentifierList(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeNonTerminal(EvalIdentifierDefinition))
    return NULL;

  while (context.ConsumeToken(Token::COMMA)) {
    if (!context.ConsumeNonTerminal(EvalIdentifierDefinition))
      // TODO - record an error here;
      return NULL;
  }

  context.node()->set_type(ParseNode::IDENTIFIER_DEFINITION_LIST);
  return context.Release();
}

// chip name;
// chip name[NUMBER];
// chip name, name2;
ParseNode* EvalChipInstance(BufferedTokenStream& input_stream,
                            ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);
  if (!context.ConsumeToken(Token::CHIP))
    return NULL;

  if (!context.ConsumeNonTerminal(EvalIdentifierList))
    return NULL;

  if (!context.ConsumeToken(Token::SEMI_COLON))
    return NULL;

  context.node()->set_type(ParseNode::CHIP_INSTANCE);
  return context.Release();
}

// wire name;
// wire name[NUMBER];
// wire name, name2;
ParseNode* EvalWireInstance(BufferedTokenStream& input_stream,
                            ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);
  if (!context.ConsumeToken(Token::WIRE))
    return NULL;

  if (!context.ConsumeNonTerminal(EvalIdentifierList))
    return NULL;

  if (!context.ConsumeToken(Token::SEMI_COLON))
    return NULL;

  context.node()->set_type(ParseNode::WIRE_INSTANCE);
  return context.Release();
}

// R-values can include chip instantiations
ParseNode* EvalRValue(BufferedTokenStream& input_stream,
                      ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
    if (!context.ConsumeNonTerminal(EvalChipIdentifierReferenceList))
      return NULL;

    if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE))
      return NULL;

    context.node()->set_type(ParseNode::RVALUE_LIST);
    return context.Release();
  }

  if (!context.ConsumeNonTerminal(EvalChipIdentifierReferenceList))
    return NULL;

  context.node()->set_type(ParseNode::RVALUE);
  return context.Release();
}

// L value is a list of references with no implicit chip calls
ParseNode* EvalLValue(BufferedTokenStream& input_stream,
                      ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (context.ConsumeToken(Token::LEFT_SQUARE_BRACE)) {
    if (!context.ConsumeNonTerminal(EvalIdentifierReferenceList))
      return NULL;

    if (!context.ConsumeToken(Token::RIGHT_SQUARE_BRACE))
      return NULL;

    context.node()->set_type(ParseNode::LVALUE_LIST);
    return context.Release();
  }

  if (!context.ConsumeNonTerminal(EvalIdentifierReference))
    return NULL;

  context.node()->set_type(ParseNode::LVALUE);
  return context.Release();
}

ParseNode* EvalLeftAssignStatement(BufferedTokenStream& input_stream,
                                   ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeNonTerminal(EvalLValue))
    return NULL;

  if (!context.ConsumeToken(Token::LEFT_ARROW))
    return NULL;

  if (!context.ConsumeNonTerminal(EvalRValue))
    return NULL;

  if (!context.ConsumeToken(Token::SEMI_COLON))
    return NULL;

  context.node()->set_type(ParseNode::LEFT_ASSIGN_STATEMENT);
  return context.Release();
}

ParseNode* EvalRightAssignStatement(BufferedTokenStream& input_stream,
                                   ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeNonTerminal(EvalRValue))
    return NULL;

  if (!context.ConsumeToken(Token::RIGHT_ARROW))
    return NULL;

  if (!context.ConsumeNonTerminal(EvalLValue))
    return NULL;

  if (!context.ConsumeToken(Token::SEMI_COLON))
    return NULL;

  context.node()->set_type(ParseNode::RIGHT_ASSIGN_STATEMENT);
  return context.Release();
}

ParseNode* EvalChipBody(BufferedTokenStream& input_stream,
                        ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  static NonTerminalEvaluator chip_body_statements[] = {
      EvalChipInstance,
      EvalWireInstance,
      EvalLeftAssignStatement,
      EvalRightAssignStatement,
      NULL
    };

  while (context.ConsumeNonTerminal(chip_body_statements)) {
  }

  if (Token::RIGHT_BRACE != input_stream.Peek().type())
    return NULL;

  context.node()->set_type(ParseNode::CHIP_BODY);
  return context.Release();
}

// Returns Null on failure
ParseNode* EvalChipDefinition(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeToken(Token::CHIP))
    return NULL;

  if (!context.ConsumeToken(Token::IDENTIFIER))
    return NULL;

  if (!context.ConsumeToken(Token::LEFT_PAREN))
    return NULL;

  if (!context.ConsumeNonTerminal(EvalIdentifierList))
    return NULL;

  if (!context.ConsumeToken(Token::RIGHT_PAREN))
    return NULL;

  if (!context.ConsumeToken(Token::RIGHT_ARROW))
    return NULL;

  if (!context.ConsumeNonTerminal(EvalIdentifierDefinition))
    return NULL;

  if (!context.ConsumeToken(Token::LEFT_BRACE))
    return NULL;

  // Evaluate body
  if (!context.ConsumeNonTerminal(EvalChipBody))
    return NULL;

  if (!context.ConsumeToken(Token::RIGHT_BRACE))
    return NULL;

  context.node()->set_type(ParseNode::CHIP_DEFINITION);
  return context.Release();
}

ParseNode* EvalImportClause(BufferedTokenStream& input_stream,
                            ParseErrorCollection *error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeToken(Token::IMPORT))
    return NULL;

  if (!context.ConsumeToken(Token::STRING))
    return NULL;

  if (!context.ConsumeToken(Token::SEMI_COLON))
    return NULL;

  context.node()->set_type(ParseNode::IMPORT_STATEMENT);
  return context.Release();
}

ParseNode* EvalTHDFile(BufferedTokenStream& input_stream,
                       ParseErrorCollection *error_collection) {
  ParsingContext context(input_stream, error_collection);
  while (!input_stream.IsEOS()) {
    static NonTerminalEvaluator top_level_statements[] = {
      EvalImportClause,
      EvalChipDefinition,
      NULL
    };

    while (context.ConsumeNonTerminal(top_level_statements)) {
    }
  }

  if (!input_stream.IsEOS())
    return NULL;

  context.node()->set_type(ParseNode::THD_MODULE);
  return context.Release();
}

}  // namespace

void ParserEntries::InitForParsing() {
  delete node_;
  delete error_collection_;

  node_ = NULL;
  error_collection_ = new ParseErrorCollection();
}

bool ParserEntries::ParseImmediateValue(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalImmediateValue(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseIdentifierDefinition(
    BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalIdentifierDefinition(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseNumberOrRange(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalNumberOrRange(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseNumberList(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalNumberList(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseIdentifierReference(
    BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalIdentifierReference(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseChipIdentifierReference(
    BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalChipIdentifierReference(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseChipIdentifierReferenceList(
    BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalChipIdentifierReferenceList(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseIdentifierReferenceList(
    BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalIdentifierReferenceList(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseIdentifierList(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalIdentifierList(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseChipInstance(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalChipInstance(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseWireInstance(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalWireInstance(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseRValue(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalRValue(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseLValue(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalLValue(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseLeftAssignStatement(
    BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalLeftAssignStatement(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseRightAssignStatement(
    BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalRightAssignStatement(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseChipBody(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalChipBody(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseChipDefinition(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalChipDefinition(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseImportClause(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalImportClause(input_stream, error_collection_);
  return NULL != node_;
}

bool ParserEntries::ParseTHDFile(BufferedTokenStream& input_stream) {
  InitForParsing();
  node_ = EvalTHDFile(input_stream, error_collection_);
  return NULL != node_;
}

Parser::Parser(BufferedTokenStream &input_stream)
  : input_stream_(input_stream) {
}

bool Parser::Evaluate() {
  return entries_.ParseTHDFile(input_stream_);
}

#if 0
bool EvalTHDModule(BufferedTokenStream& input_stream,
                   ParseNode* root,
                   ParseErrorCollection *error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, root);

  return false;
}
#endif

#if 0
int main(int argc, char* argv[]) {
  std::ifstream fstream("example.thd");

  if (!fstream) {
    return 0;
  }

  IoStream io_stream(&fstream);

  LineCountingStream line_count(io_stream);
  size_t current_count = line_count.GetCount();

  TokenStream token_stream(line_count);

  BufferedTokenStream buffered_stream(token_stream);

  //ParseNode* chip_node = new ParseNode;
  ParseErrorCollection errors;

  ParseNode* chip_node = EvalTHDFile(buffered_stream, &errors);

  std::cout << *chip_node;

#if 0
  while (!token_stream.IsEOS()) {
    Token token = token_stream.Get();
    std::cout << "Token:= " << Token::kTokenTypeNames[token.type()] << ": "
        << token.value() << ": Line :" << token.line() << std::endl;
  }
#endif

  return 0;
}
#endif
