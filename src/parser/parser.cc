
#include "base/base.h"
#include "lexer/token_stream.h"
#include "lexer/line_counting_stream.h"
#include "streams/iostream.h"
#include "streams/buffered_stream.h"

#include <fstream>

#include <iostream>
#include <vector>

#include "parser/parser_utils.h"
#include "parser/parse_node.h"

typedef BufferedStream<Token> BufferedTokenStream;

class ParserInput {
 public:
  ParserInput(BufferedTokenStream* buffered_stream, TokenStream* token_stream)
      : buffered_stream_(buffered_stream), inner_stream_(token_stream) {}

  BufferedTokenStream* buffered_stream() { return buffered_stream_; }
  TokenStream* inner_stream() { return inner_stream_; }

 private:
  BufferedTokenStream* buffered_stream_;
  TokenStream* inner_stream_;

  DISALLOW_COPY_AND_ASSIGN(ParserInput);
};

class ParseError {
 public:
  ParseError(Token const & token, std::string const & message)
      : token_(token), message_(message) {}

  Token const & token() const { return token_; }
  std::string const & message() const { return message_; }

 private:
  Token token_;
  std::string message_;
};

class ParseErrorCollection {
 public:
  typedef std::vector<ParseError> ErrorArray;

  void RegisterError(Token const & token, std::string const & message) {
    parse_errors_.push_back(ParseError(token, message));
  }

  ErrorArray const & errors() const { return parse_errors_; }

 private:
  ErrorArray parse_errors_;

  DISALLOW_COPY_AND_ASSIGN(ParseErrorCollection);
};

bool ConsumeToken(BufferedTokenStream& input_stream, ParseNode* parse_node,
                  Token::TokenType token_type) {
  Token token = NextToken(input_stream);

  if (Token::UNKNOWN == token.type())
    return false;

  if (token_type == token.type()) {
    parse_node->PushTerminal(token);
    return true;
  }

  // correct the stream
  input_stream.Unget(token);
  return false;
}

typedef bool (*NonTerminalEvaluator)(BufferedTokenStream&, ParseNode*,
                                     ParseErrorCollection*);

bool ConsumeNonTerminal(BufferedTokenStream& input_stream,
                        ParseNode* parse_node,
                        ParseErrorCollection* error_collection,
                        NonTerminalEvaluator evaluator) {
  ParseNode* non_terminal = new ParseNode;
  if (!evaluator(input_stream, non_terminal, error_collection)) {
    delete non_terminal;
    return false;
  }

  parse_node->PushNonTerminal(non_terminal);
  return true;
}

void UnwindParseNode(BufferedTokenStream* stream, ParseNode const* parse_node) {
  if (!stream || !parse_node)
    return;

  // Starting at the last element, unget all of the productions in order
  class UnwindVisitor : public ParseNode::Visitor {
   public:
    UnwindVisitor(BufferedTokenStream* stream) : stream_(stream) {}

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

class AutoTokenConsumer {
 public:
  AutoTokenConsumer(BufferedTokenStream* stream) : stream_(stream) {}
  ~AutoTokenConsumer();

  // Call to prevent restoring of the consumed contents back into the stream.
  void Release(ParseNode* node) {
    if (node) {
      //node->terminals() = consumed_;
    }
    stream_ = NULL;
  }

  Token NextToken() {
    consumed_.push_back(stream_->Get());
    return *consumed_.rbegin();
  }

 private:
  BufferedTokenStream* stream_;
  std::vector<Token> consumed_;

  DISALLOW_COPY_AND_ASSIGN(AutoTokenConsumer);
};

AutoTokenConsumer::~AutoTokenConsumer() {
  if (stream_) {
    for (int x = consumed_.size() - 1; x > 0; --x)
      stream_->Unget(consumed_[x-1]);
  }
}

struct PinDefinition {
 int pin_count_;
 std::string name_;
};

bool EvalPinDefinition(BufferedTokenStream& input_stream,
                       ParseNode* pin_definition,
                       ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, pin_definition);
  if (!ConsumeToken(input_stream, pin_definition, Token::IDENTIFIER))
    return false;

  if (!ConsumeToken(input_stream, pin_definition, Token::LEFT_SQUARE_BRACE)) {
    pin_definition->set_type(ParseNode::SINGLE_PIN_DEFINITION);
    auto_unwind.Release();
    return true;
  }

  if (!ConsumeToken(input_stream, pin_definition, Token::NUMBER_BINARY) &&
      !ConsumeToken(input_stream, pin_definition, Token::NUMBER_DECIMAL) &&
      !ConsumeToken(input_stream, pin_definition, Token::NUMBER_HEX))
    return false;

  if (!ConsumeToken(input_stream, pin_definition, Token::RIGHT_SQUARE_BRACE))
    return false;

  pin_definition->set_type(ParseNode::ARRAY_PIN_DEFINITION);
  auto_unwind.Release();
  return true;
}

bool EvalArgList(BufferedTokenStream& input_stream,
                 ParseNode* arg_list,
                 ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, arg_list);

  if (!ConsumeNonTerminal(input_stream, arg_list, error_collection,
                          EvalPinDefinition))
    return false;

  while (ConsumeToken(input_stream, arg_list, Token::COMMA)) {
    if (!ConsumeNonTerminal(input_stream, arg_list, error_collection,
                            EvalPinDefinition)) {
      // TODO - record an error here;
      return false;
    }
  }

  arg_list->set_type(ParseNode::ARGUMENT_DEFINITION);
  auto_unwind.Release();
  return true;
}

// Returns Null on failure
bool EvalChipDescription(BufferedTokenStream& input_stream,
                         ParseNode* chip_description,
                         ParseErrorCollection* error_collection) {
  AutoParseNodeUnwinder auto_unwind(&input_stream, chip_description);

  if (!ConsumeToken(input_stream, chip_description, Token::CHIP))
    return false;

  if (!ConsumeToken(input_stream, chip_description, Token::IDENTIFIER))
    return false;

  if (!ConsumeToken(input_stream, chip_description, Token::LEFT_PAREN))
    return false;

  if (!ConsumeNonTerminal(input_stream, chip_description, error_collection,
                          EvalArgList))
    return false;

  if (!ConsumeToken(input_stream, chip_description, Token::RIGHT_PAREN))
    return false;

  if (!ConsumeToken(input_stream, chip_description, Token::RIGHT_ARROW))
    return false;

  if (!ConsumeNonTerminal(input_stream, chip_description, error_collection,
                          EvalPinDefinition))
    return false;

  if (!ConsumeToken(input_stream, chip_description, Token::LEFT_BRACE))
    return false;

  // Evaluate body

  if (!ConsumeToken(input_stream, chip_description, Token::RIGHT_BRACE))
    return false;

  chip_description->set_type(ParseNode::CHIP_DESCRIPTION);
  auto_unwind.Release();
  return true;
}

bool EvalTHD(TokenStream& input_stream) {
  return false;
}

int main(int argc, char* argv[]) {
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

  return 0;
}
