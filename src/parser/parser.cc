
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
                       ParseNode* pin_definition) {
  AutoTokenConsumer token_consumer(&input_stream);
  Token token = token_consumer.NextToken();
  if (Token::IDENTIFIER != token.type())
    return false;

  token = token_consumer.NextToken();
  if (Token::LEFT_SQUARE_BRACE != token.type()) {
    token_consumer.Release(pin_definition);
    pin_definition->set_type(ParseNode::PIN_DEFINITION);
    return true;
  }

  token = token_consumer.NextToken();
  if (!IsTokenNumeric(token))
    return false;

  token = input_stream.Get();
  if (Token::RIGHT_SQUARE_BRACE != token.type())
    return false;

  token_consumer.Release(pin_definition);
  pin_definition->set_type(ParseNode::PIN_DEFINITION);

  return true;
}

bool EvalArgList(BufferedTokenStream& input_stream,
                 ParseNode* arg_list) {
  ParseNode argument;
  if (!EvalPinDefinition(input_stream, &argument))
    return false;

  //arg_list->non_terminals().push_back(argument);

  AutoTokenConsumer token_consumer(&input_stream);
  Token token = token_consumer.NextToken();
  while (Token::COMMA == token.type()) {
    if (!EvalPinDefinition(input_stream, &argument))
      break;

    //arg_list->non_terminals().push_back(argument);
    token = token_consumer.NextToken();
  }

  token_consumer.Release(arg_list);
  arg_list->set_type(ParseNode::ARGUMENT_DEFINITION);

  return true;
}

// Returns Null on failure
bool EvalChipDescription(BufferedTokenStream& input_stream,
                         ParseNode* chip_description) {
  AutoTokenConsumer token_consumer(&input_stream);

  Token token = token_consumer.NextToken();
  if (Token::CHIP != token.type())
    return false;

  token = token_consumer.NextToken();
  if (Token::IDENTIFIER != token.type())  // The chip name
    return false;

  token = token_consumer.NextToken();
  if (Token::LEFT_PAREN != token.type())
    return false;

  //call Eval ArgList
  //chip_description->

  token = token_consumer.NextToken();
  if (Token::RIGHT_PAREN != token.type())
    return false;

  token = input_stream.Get();
  if (Token::RIGHT_ARROW != token.type())
    return false;

  token = input_stream.Get();

  return false;
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
