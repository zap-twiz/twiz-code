
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


typedef BufferedStream<Token> BufferedTokenStream;

void UnwindParseNode(BufferedTokenStream* stream, ParseNode const* parse_node) {
  if (!stream || !parse_node)
    return;

  // Starting at the last element, unget all of the productions in order
  class UnwindVisitor : public ParseNode::Visitor {
   public:
    explicit UnwindVisitor(BufferedTokenStream* stream) : stream_(stream) {}

    virtual void VisitTerminal(Token const & token, size_t offset) {
      stream_->Unget(token);
    }
    virtual void VisitNonTerminal(ParseNode const * parse_node, size_t offset) {
      UnwindParseNode(stream_, parse_node);
    }

   private:
    BufferedTokenStream* stream_;
    DISALLOW_COPY_AND_ASSIGN(UnwindVisitor);
  };

  UnwindVisitor unwind_visitor(stream);
  parse_node->VisitChildrenRightToLeft(&unwind_visitor);
}

class AutoParseNodeUnwinder {
 public:
  AutoParseNodeUnwinder(BufferedTokenStream* stream, ParseNode* parse_node)
      : stream_(stream), parse_node_(parse_node) {}
  ~AutoParseNodeUnwinder() {
    if (stream_ && parse_node_)
      UnwindParseNode(stream_, parse_node_);
  }

  void Release() { stream_ = NULL; parse_node_ = NULL; }

 private:
  BufferedTokenStream* stream_;
  ParseNode* parse_node_;

  DISALLOW_COPY_AND_ASSIGN(AutoParseNodeUnwinder);
};

// Returns true if the token was matched, false otherwise
bool ConsumeToken(BufferedTokenStream& input_stream, ParseNode* parse_node,
                  Token::TokenType token_type) {
  Token token = NextToken(input_stream);

  if (Token::UNKNOWN == token.type())
    return false;

  if (token_type == token.type()) {
    parse_node->PushTerminal(token);
    return true;
  }

  // Correct the stream.
  input_stream.Unget(token);
  return false;
}

bool ConsumeToken(BufferedTokenStream& input_stream, ParseNode* parse_node,
                  Token::TokenType* token_types) {
  Token token = NextToken(input_stream);

  if (Token::UNKNOWN == token.type())
    return false;

  for (Token::TokenType* token_type = token_types; *token_type; ++token_type) {
    if (token.type() == *token_type) {
      parse_node->PushTerminal(token);
      return true;
    }
  }

  // Correct the stream.
  input_stream.Unget(token);
  return false;
}

typedef ParseNode* (*NonTerminalEvaluator)(
    BufferedTokenStream&, ParseErrorCollection*);

bool ConsumeNonTerminal(BufferedTokenStream& input_stream,
                        ParseNode* parent_node,
                        ParseErrorCollection* error_collection,
                        NonTerminalEvaluator evaluator) {
  ParseNode* result = evaluator(input_stream, error_collection);
  if (result) {
    parent_node->PushNonTerminal(result);
    return true;
  }
  return false;
}

ParseNode* ConsumeNonTerminal(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection,
                              NonTerminalEvaluator* evaluators) {
  ParseErrorCollection local_collection;
  ParseNode* node = NULL;
  for (NonTerminalEvaluator* evaluator = evaluators; *evaluator; ++evaluator) {
    if (node = (*evaluator)(input_stream, &local_collection)) {
      return node;
    }
  }

  // None of the evaluators matched, so return all of the errors.
  error_collection->RegisterErrorCollection(local_collection);
  return NULL;
}

bool ConsumeNonTerminal(BufferedTokenStream& input_stream,
                        ParseNode* parent_node,
                        ParseErrorCollection* error_collection,
                        NonTerminalEvaluator* evaluators) {
  ParseNode* result = ConsumeNonTerminal(input_stream, error_collection,
                                         evaluators);
  if (result) {
    parent_node->PushNonTerminal(result);
    return true;
  }
  return false;
}

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
    if (!node_)
      unwinder_.Release();

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

#if 0
class ParsingContext {
 public:
  ParsingContext(BufferedTokenStream& input_stream)
      : input_stream_(input_stream),
        root_(NULL),
        error_collection_(new ParseErrorCollection) {
  }

  BufferedTokenStream& input_stream() { return input_stream_; }
  ParseNode* root() { return root_; }
  ParseErrorCollection* error_collection() { return error_collection_; }

  ParseNode* EvalPinDefinition() {
    std::auto_ptr<ParseNode> pin_definition(new ParseNode);
    AutoParseNodeUnwinder auto_unwind(input_stream(), pin_definition.get());
    if (!ConsumeToken(input_stream, pin_definition.get(), Token::IDENTIFIER)) {
      error_collection
      return NULL;
    }

    if (!ConsumeToken(input_stream, pin_definition.get(), Token::LEFT_SQUARE_BRACE)) {
      pin_definition->set_type(ParseNode::SINGLE_PIN_DEFINITION);
      auto_unwind.Release();
      return pin_definition.release();
    }

    if (!ConsumeToken(input_stream, pin_definition.get(), Token::NUMBER_BINARY) &&
        !ConsumeToken(input_stream, pin_definition.get(),
                      Token::NUMBER_DECIMAL) &&
        !ConsumeToken(input_stream, pin_definition.get(), Token::NUMBER_HEX)) {
      return NULL;
    }

    if (!ConsumeToken(input_stream, pin_definition.get(),
                      Token::RIGHT_SQUARE_BRACE)) {
      return NULL;
    }

    pin_definition->set_type(ParseNode::ARRAY_PIN_DEFINITION);
    auto_unwind.Release();
    return pin_definition.release();
  }

 private:
  BufferedTokenStream& input_stream_;
  ParseNode* root_;
  ParseErrorCollection* error_collection_;
};
#endif

#if 0
ParseNode* ConsumeNonTerminal(
    BufferedTokenStream& input_stream, ParseErrorCollection* error_collection,
    std::vector<NonTerminalEvaluator> const & evaluators) {

  ParseNode* parse_node = NULL;


#if 0
  std::vector<NonTerminalEvaluator>::const_iterator iter(evaluators.begin()),
      end(evaluators.end());
  for (; iter != end; ++iter) {
    (*iter)(input_stream, error_collection
  }
#endif

  return NULL;
}
#endif

#if 0
ParseNode* ConsumeNonTerminal(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection,
                              NonTerminalEvaluator evaluator) {
  ParseNode* non_terminal = new ParseNode;
  ParseNode::ProductionType production_type =
      evaluator(input_stream, non_terminal, error_collection);
  if (ParseNode::UNKNOWN == production_type)
    delete non_terminal;

  return non_termainal;
}
#endif

// Identifier Definition
// name
// name[NUMBER]
ParseNode* EvalIdentifierDefinition(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);
  if (!context.ConsumeToken(Token::IDENTIFIER)) {
    return NULL;
  }

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
// name[number]
// name.pin
// name[number].pin[number]
ParseNode* EvalIdentifierReference(BufferedTokenStream& input_stream,
                                   ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeToken(Token::IDENTIFIER))
    return NULL;

  if (context.ConsumeToken(Token::LEFT_PAREN)) {
    if (!context.ConsumeNonTerminal(EvalNumberList))
      return NULL;

    if (!context.ConsumeToken(Token::RIGHT_PAREN))
      return NULL;
  }

  if (context.ConsumeToken(Token::DOT)) {
    if (!context.ConsumeToken(Token::IDENTIFIER))
      return NULL;

    if (context.ConsumeToken(Token::LEFT_PAREN)) {
      if (!context.ConsumeNonTerminal(EvalNumberList))
        return NULL;

      if (!context.ConsumeToken(Token::RIGHT_PAREN))
        return NULL;
    }
  }

  context.node()->set_type(ParseNode::IDENTIFIER_REFERENCE);
  return context.Release();
}

ParseNode* EvalIdentifierList(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeNonTerminal(EvalIdentifierDefinition))
    return NULL;

  while (context.ConsumeToken(Token::COMMA)) {
    if (!context.ConsumeNonTerminal(EvalIdentifierDefinition)) {
      // TODO - record an error here;
      return NULL;
    }
  }

  context.node()->set_type(ParseNode::IDENTIFIER_DEFINITION_LIST);
  return context.Release();
}

#if 0
bool EvalWireDeclaration(BufferedTokenStream& input_stream,
                         ParseNode* wire_declaration,
                         ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, wire_declaration);

  if (!ConsumeToken(input_stream, wire_declaration, Token::WIRE))
    return false;

  if (!ConsumeNonTerminal(input_stream, wire_declaration, error_collection,
                          EvalPinDefinition))
    return false;

  if (!ConsumeToken(input_stream, wire_declaration, Token::SEMI_COLON))
    return false;

  wire_declaration->set_type(ParseNode::WIRE_DECLARATION);
  auto_unwind.Release();
  return true;
}

bool EvalChipDeclaration(BufferedTokenStream& input_stream,
                         ParseNode* chip_declaration,
                         ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, chip_declaration);

  if (!ConsumeToken(input_stream, chip_declaration, Token::CHIP))
    return false;

  // Only allow for a single instance?
  if (!ConsumeToken(input_stream, chip_declaration, Token::IDENTIFIER))
    return false;

  if (!ConsumeToken(input_stream, chip_declaration, Token::SEMI_COLON))
    return false;

  chip_declaration->set_type(ParseNode::CHIP_DECLARATION);
  auto_unwind.Release();
  return true;
}

bool EvalNumberRange(BufferedTokenStream& input_stream,
                     ParseNode* range,
                     ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, range);

  // Number ranges only allow decimal numbers
  if (!ConsumeToken(input_stream, range, Token::NUMBER_DECIMAL))
    return false;

  if (!ConsumeToken(input_stream, range, Token::DOT_DOT))
    return false;

  if (!ConsumeToken(input_stream, range, Token::NUMBER_DECIMAL))
    return false;

  range->set_type(ParseNode::NUMBER_RANGE);
  auto_unwind.Release();
  return true;
}

bool EvalNumberOrNumberRange(BufferedTokenStream& input_stream,
                             ParseNode* number_or_range,
                             ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, number_or_range);

  ParseErrorCollection local_errors;
  if (ConsumeNonTerminal(input_stream, number_or_range, &local_errors,
                         EvalNumberRange)) {
    auto_unwind.Release();
    return true;
  }

  if (ConsumeToken(input_stream, number_or_range, Token::NUMBER_DECIMAL) ||
      ConsumeToken(input_stream, number_or_range, Token::NUMBER_HEX) ||
      ConsumeToken(input_stream, number_or_range, Token::NUMBER_BINARY)) {
    number_or_range->set_type(ParseNode::NUMBER);
    auto_unwind.Release();
    return true;
  }
  
  return false;
}

bool EvalNumberCollection(BufferedTokenStream& input_stream,
                          ParseNode* number_collection,
                          ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, number_collection);

  if (!ConsumeNonTerminal(input_stream, number_collection, error_collection,
                          EvalNumberOrNumberRange))
    return false;

  while(ConsumeToken(input_stream, number_collection, Token::COMMA)){
    if (!ConsumeNonTerminal(input_stream, number_collection, error_collection,
                            EvalNumberOrNumberRange))
        return false;
  }

  number_collection->set_type(ParseNode::NUMBER_COLLECTION);
  auto_unwind.Release();
  return true;
}

bool EvalAssignmentLValue(BufferedTokenStream& input_stream,
                          ParseNode* l_value,
                          ParseErrorCollection* error_collection) {
  return false;
}

bool EvalAssignmentRValue(BufferedTokenStream& input_stream,
                          ParseNode* r_value,
                          ParseErrorCollection* error_collection) {
  return false;
}

bool EvalLeftPinAssignStatement(BufferedTokenStream& input_stream,
                                ParseNode* statement,
                                ParseErrorCollection* error_collection) {
  return false;
}

bool EvalPinAssignStatement(BufferedTokenStream& input_stream,
                            ParseNode* statement,
                            ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, statement);

  ParseErrorCollection local_errors;
  if (ConsumeNonTerminal(input_stream, statement, &local_errors,
                         EvalLeftPinAssignStatement)) {
    // assign a statement type
    auto_unwind.Release();
    return true;
  }

#if 0
  if (ConsumeNonTerminal(input_stream, statement, error_collection,
                         EvalRightPinAssignStatement)) {
    // assign a statement type
    auto_unwind.Release();
    return true;
  }
#endif

  error_collection->RegisterErrorCollection(local_errors);
  return false;
}

bool EvalChipBody(BufferedTokenStream& input_stream,
                  ParseNode* chip_body,
                  ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, chip_body);

  bool valid_body = false;
  while (true) {
    ParseErrorCollection local_errors;
    if (ConsumeNonTerminal(input_stream, chip_body, &local_errors,
                          EvalChipDeclaration))
      continue;

    if (ConsumeNonTerminal(input_stream, chip_body, &local_errors,
                           EvalWireDeclaration))
      continue;

    if (Token::RIGHT_BRACE == input_stream.Peek().type()) {
      valid_body = true;
      break;
    }
  }

  auto_unwind.Release();
  return valid_body;
}
#endif

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

ParseNode* EvalRValue(BufferedTokenStream& input_stream,
                      ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);
  return context.Release();
}

ParseNode* EvalLValue(BufferedTokenStream& input_stream,
                      ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  return context.Release();
}

ParseNode* EvalLeftAssignStatement(BufferedTokenStream& input_stream,
                                   ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  if (!context.ConsumeNonTerminal(EvalLValue))
    return NULL;

  if (!context.ConsumeToken(Token::RIGHT_ARROW))
    return NULL;

  if (!context.ConsumeNonTerminal(EvalRValue))
    return NULL;

  if (!context.ConsumeToken(Token::SEMI_COLON))
    return NULL;

  context.node()->set_type(ParseNode::LEFT_ASSIGN_STATEMENT);
  return context.Release();
}

ParseNode* EvalChipBody(BufferedTokenStream& input_stream,
                        ParseErrorCollection* error_collection) {
  ParsingContext context(input_stream, error_collection);

  static NonTerminalEvaluator chip_body_statements[] = {
      EvalChipInstance,
      EvalWireInstance,
      EvalLeftAssignStatement,
      NULL
    };
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
  while(!input_stream.IsEOS()) {
    static NonTerminalEvaluator top_level_statements[] = {
      EvalImportClause,
      EvalChipDefinition,
      NULL
    };

    if (!context.ConsumeNonTerminal(top_level_statements)) {
      // TODO:  Specify some error recovery logic.
      return NULL;
    }
  }

  context.node()->set_type(ParseNode::THD_MODULE);
  return context.Release();
}

#if 0
bool EvalTHDModule(BufferedTokenStream& input_stream,
                   ParseNode* root,
                   ParseErrorCollection *error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, root);

  return false;
}
#endif

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

#if 0
  while (!token_stream.IsEOS()) {
    Token token = token_stream.Get();
    std::cout << "Token:= " << Token::kTokenTypeNames[token.type()] << ": "
        << token.value() << ": Line :" << token.line() << std::endl;
  }
#endif

  return 0;
}
