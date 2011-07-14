#ifndef INCLUDED_PARSER_PARSE_ERROR_H_
#define INCLUDED_PARSER_PARSE_ERROR_H_

#include "lexer/token.h"

#include <string>


// Todo:  Fix the error reporting structure
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

#endif  // INCLUDED_PARSER_PARSE_ERROR_H_
