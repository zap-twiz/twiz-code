#ifndef INCLUDED_LEXER_TOKEN_H_
#define INCLUDED_LEXER_TOKEN_H_

#include <string>

class Token {
 public:
  typedef enum TokenType {
    UNKNOWN = 0,
    END_OF_STREAM,
    RIGHT_SQUARE_BRACE,
    LEFT_SQUARE_BRACE,
    RIGHT_PAREN,
    LEFT_PAREN,
    RIGHT_BRACE,
    LEFT_BRACE,
    DOT,
    DOT_DOT,
    COMMA,
    SEMI_COLON,
    COLON,
    RIGHT_ARROW,
    LEFT_ARROW,
    CHIP,
    IMPORT,
    WIRE,
    TRUE,
    FALSE,
    HIGH,
    LOW,
    IDENTIFIER,
    NUMBER_DECIMAL,
    NUMBER_HEX,
    NUMBER_BINARY,
    STRING,
    COMMENT,
    TEST,
    NUM_TOKEN_TYPES
  };

  static char const * const kTokenTypeNames[];

  Token(int line, TokenType type, const std::string& value)
    : line_(line), type_(type), value_(value) {}

  TokenType type() const { return type_; }

  std::string const & value() const { return value_; }

  int line() const { return line_; }

 private:
  int line_;
  TokenType type_;
  std::string value_;

  // Disallow default construction
  Token() {};
};

#endif  // INCLUDED_LEXER_LEX_TOKEN_H_
