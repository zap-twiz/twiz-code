#include "gtest/gtest.h"

#include "lexer/token.h"

namespace {

TEST(TokenTest, Basic) {
  Token token(0, Token::UNKNOWN, "42");
  EXPECT_EQ(0, token.line());
  EXPECT_TRUE(Token::UNKNOWN == token.type());
  EXPECT_TRUE(token.value() == "42");

  EXPECT_STREQ("UNKNOWN", Token::kTokenTypeNames[Token::UNKNOWN]);
}

}  // namespace
