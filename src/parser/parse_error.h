#ifndef INCLUDED_PARSER_PARSE_ERROR_H_
#define INCLUDED_PARSER_PARSE_ERROR_H_

#include "lexer/token.h"

#include <string>


// Todo:  Fix the error reporting structure
class ParseError {
 public:
  ParseError(Token const & token, std::string const & message)
      : token_(token), message_(message) {}
  ~ParseError() {}

  Token const & token() const { return token_; }
  std::string const & message() const { return message_; }

  ParseError& operator=(ParseError const & parse_error) {
    token_ = parse_error.token_;
    message_ = parse_error.message_;
    return *this;
  }

 private:
  Token token_;
  std::string message_;

  // Disallow default construction.
  ParseError();
};

#endif  // INCLUDED_PARSER_PARSE_ERROR_H_
