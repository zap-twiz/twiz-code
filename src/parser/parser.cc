
#include "lexer/token_stream.h"
#include "lexer/line_counting_stream.h"
#include "streams/iostream.h"

#include <fstream>

#include <iostream>
#include <vector>

class ParseTree {
};

class TreeNode {
}

class ErrorLog {
};

class THDParser {
};

namespace Parser {

class PinDefinition {
 public:
  PinDefinition(int pin_count, std::string const & name)
      : pin_count_(pin_count), name_(name) {}

  int pin_count() const { return pin_count; }
  std::string const & name() const { return name_; }
 private:
 int pin_count_;
 std::string name_;
};

class ArgumentDefinition {
 public:
 private:
  std::vector<PinDefinition> pins_;
};

class PinRange {
};

class PinCollection {
};

class ChipBody {
 public:
 private:
}

class ChipDefinition {
 private:
  
}; 

}  // namespace Parse

bool IsTokenIgnored(Token const & token) {
  return Token::COMMENT == token.type();
}

bool IsTokenNumeric(Token const & token) {
  return Token::NUMBER_BINARY == token.type() ||
      Token::NUMBER_DECIMAL == token.type() ||
      Token::NUMBER_HEX == token.type();
}

bool NextToken(TokenStream& input_stream, Token* next_token) {
  if (input_stream.IsEOS())
    return false;

  Token token = input_stream.Get();
  while (IsTokenIgnored(token)) {
    if (input_stream.IsEOS())
      break;
    token = input_stream.Get();
  }

  *next_token = token;
  return input_stream.IsEOS();
}

struct PinDefinition {
 int pin_count_;
 std::string name_;
};

bool EvalPinDefinition(TokenStream& input_stream,
                       PinDefinition* pin_definition) {
  Token token = input_stream.Get();
  if (Token::IDENTIFIER != token.type())
    return false;

  token = input_stream.Get();
  if (Token::LEFT_SQUARE_BRACE == token.type()) {
  }
  
  token = input_stream.Get();
  if (IsTokenNumeric(token)) {
  }
  
  token = input_stream.Get();
  if (Token::RIGHT_SQUARE_BRACE == token.type()) {
  }
  
  return false;
}

bool EvalArgList(TokenStream& input_stream) {
  return false;
}

bool EvalChip(TokenStream& input_stream) {
  std::vector<Token> consumed;
  Token token = input_stream.Get();
  if (Token::CHIP != token.type()) {
    return false;
  }
  consumed.push_back(token);

  token = input_stream.Get();
  if (Token::IDENTIFIER != token.type())  // The chip name
    return false;
  consumed.push_back(token);

  token = input_stream.Get();
  if (Token::LEFT_PAREN != token.type())
    return false;
  consumed.push_back(token);

  // call Eval ArgList

  token = input_stream.Get();
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
