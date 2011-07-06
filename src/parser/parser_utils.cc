
#include "parser/parser_utils.h"

#include "lexer/token.h"
#include "streams/stream.h"

bool IsTokenIgnored(Token const & token) {
  return Token::COMMENT == token.type();
}

bool IsTokenNumeric(Token const & token) {
  return Token::NUMBER_BINARY == token.type() ||
      Token::NUMBER_DECIMAL == token.type() ||
      Token::NUMBER_HEX == token.type();
}

Token NextToken(Stream<Token>& input_stream) {
  if (input_stream.IsEOS())
    return Token(-1, Token::UNKNOWN, "");

  Token token = input_stream.Get();
  bool found = true;
  while (IsTokenIgnored(token)) {
    if (input_stream.IsEOS()) {
      found = false;
      break;
    }
    token = input_stream.Get();
  }

  if (!found)
    return Token(token.line(), Token::UNKNOWN, "");

  return token;
}

