
#include "lexer/token.h"

char const * const Token::kTokenTypeNames[] = {
  "UNKNOWN",
  "END_OF_STREAM",
  "RIGHT_SQUARE_BRACE",
  "LEFT_SQUARE_BRACE",
  "RIGHT_PAREN",
  "LEFT_PAREN",
  "RIGHT_BRACE",
  "LEFT_BRACE",
  "DOT",
  "DOT_DOT",
  "COMMA",
  "SEMI_COLON",
  "COLON",
  "RIGHT_ARROW",
  "LEFT_ARROW",
  "CHIP",
  "IMPORT",
  "WIRE",
  "TRUE",
  "FALSE",
  "HIGH",
  "LOW",
  "IDENTIFIER",
  "NUMBER_DECIMAL",
  "NUMBER_HEX",
  "NUMBER_STRING",
  "STRING",
  "COMMENT",
  "TEST"
};
