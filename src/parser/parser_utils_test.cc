#include "gtest/gtest.h"

#include "parser/parser_utils.h"

#include "lexer/line_counting_stream.h"
#include "lexer/token_stream.h"
#include "streams/iostream.h"

#include <sstream>

namespace {

class ParserUtilsTest : public ::testing::Test {
 public:
  struct StreamSet {
    std::stringstream* string_stream_;
    IoStream* io_stream_;
    LineCountingStream* buffered_stream_;
    TokenStream* token_stream_;

    ~StreamSet() {
      delete buffered_stream_;
      delete io_stream_;
      delete string_stream_;
    }
  };

  static void BuildStream(std::string const & input,
                          StreamSet* stream_set) {
    stream_set->string_stream_ = new std::stringstream(input);
    stream_set->io_stream_ = new IoStream(stream_set->string_stream_);
    stream_set->buffered_stream_ =
        new LineCountingStream(*stream_set->io_stream_);
    stream_set->token_stream_ = new TokenStream(*stream_set->buffered_stream_);
  }
};

TEST_F(ParserUtilsTest, IsTokenIgnored) {
  for (int token_type = 0; token_type < Token::NUM_TOKEN_TYPES; ++token_type) {
    Token token(0, static_cast<Token::TokenType>(token_type), "TestValue");
    if (token_type == Token::COMMENT) {
      EXPECT_TRUE(IsTokenIgnored(token));
    } else {
      EXPECT_FALSE(IsTokenIgnored(token));
    }
  }
}

TEST_F(ParserUtilsTest, IsTokenNumeric) {
  for (int token_type = 0; token_type < Token::NUM_TOKEN_TYPES; ++token_type) {
    Token token(0, static_cast<Token::TokenType>(token_type), "TestValue");
    if (token_type == Token::NUMBER_BINARY ||
        token_type == Token::NUMBER_DECIMAL ||
        token_type == Token::NUMBER_HEX) {
      EXPECT_TRUE(IsTokenNumeric(token));
    } else {
      EXPECT_FALSE(IsTokenNumeric(token));
    }
  }
}

TEST_F(ParserUtilsTest, NextToken) {
  {
    StreamSet set;
    BuildStream("", &set);
    TokenStream* stream = set.token_stream_;

    EXPECT_EQ(NextToken(*stream).type(), Token::UNKNOWN);
    EXPECT_EQ(NextToken(*stream).type(), Token::UNKNOWN);
  }

  {
    StreamSet set;
    BuildStream("//..", &set);
    TokenStream* stream = set.token_stream_;

    EXPECT_EQ(NextToken(*stream).type(), Token::UNKNOWN);
  }

  {
    StreamSet set;
    BuildStream(
      "//..\n"
      "chip\n"
      "/* comment */\n"
      , &set);
    TokenStream* stream = set.token_stream_;

    EXPECT_EQ(NextToken(*stream).type(), Token::CHIP);
    EXPECT_EQ(NextToken(*stream).type(), Token::UNKNOWN);
  }
}

}  // namespace
