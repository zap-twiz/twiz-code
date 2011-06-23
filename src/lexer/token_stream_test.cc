
#include "gtest/gtest.h"

#include "lexer/line_counting_stream.h"
#include "lexer/token_stream.h"
#include "streams/iostream.h"

namespace {

class TokenStreamTest: public ::testing::Test {
 protected:
  struct StreamSet {
    std::stringstream* string_stream_;
    IoStream* io_stream_;
    LineCountingStream* buffered_stream_;

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
  }
};

TEST_F(TokenStreamTest, StreamConstruction) {
  StreamSet streams;
  BuildStream("", &streams);
  TokenStream token_stream(*streams.buffered_stream_);

  EXPECT_TRUE(token_stream.IsEOS());

  Token token = token_stream.Get();
  EXPECT_TRUE(Token::UNKNOWN == token.type());
  EXPECT_TRUE(1 == token.line());
  EXPECT_TRUE(token.value() == "");
}

TEST_F(TokenStreamTest, SpecialTokens) {
  StreamSet streams;
  BuildStream("][)(}{", &streams);
  TokenStream token_stream(*streams.buffered_stream_);
  EXPECT_FALSE(token_stream.IsEOS());

  Token token = token_stream.Get();
  EXPECT_TRUE(Token::RIGHT_SQUARE_BRACE == token.type());
  EXPECT_TRUE(1 == token.line());
  EXPECT_FALSE(token_stream.IsEOS());

  token = token_stream.Get();
  EXPECT_TRUE(Token::LEFT_SQUARE_BRACE == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::RIGHT_PAREN == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::LEFT_PAREN == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::RIGHT_BRACE == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::LEFT_BRACE == token.type());

  streams.string_stream_->clear();
  streams.string_stream_->str("...,;:-><-");
  streams.io_stream_->set_stream(streams.string_stream_);
  token = token_stream.Get();
  EXPECT_TRUE(Token::DOT_DOT == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::DOT == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::COMMA == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::SEMI_COLON  == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::COLON == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::RIGHT_ARROW == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::LEFT_ARROW == token.type());
}

TEST_F(TokenStreamTest, ReservedWords) {
  StreamSet streams;
  BuildStream("chip import wire TRUE FALSE HIGH LOW", &streams);
  TokenStream token_stream(*streams.buffered_stream_);
  EXPECT_FALSE(token_stream.IsEOS());

  Token token = token_stream.Get();
  EXPECT_TRUE(Token::CHIP == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::IMPORT == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::WIRE == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::TRUE == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::FALSE == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::HIGH == token.type());

  token = token_stream.Get();
  EXPECT_TRUE(Token::LOW == token.type());
}

TEST_F(TokenStreamTest, Identifiers) {
  StreamSet streams;
  BuildStream("identifier _ident x9", &streams);
  TokenStream token_stream(*streams.buffered_stream_);

  Token token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "identifier");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "_ident");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "x9");
}

TEST_F(TokenStreamTest, IdentifiersReserved) {
  StreamSet streams;
  BuildStream("chipX importX wireX TRUEX FALSEX HIGHX LOWX", &streams);
  TokenStream token_stream(*streams.buffered_stream_);
  EXPECT_FALSE(token_stream.IsEOS());

  Token token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "chipX");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "importX");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "wireX");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "TRUEX");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "FALSEX");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "HIGHX");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "LOWX");

  streams.string_stream_->clear();
  streams.string_stream_->str("_chip");
  streams.io_stream_->set_stream(streams.string_stream_);

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "_chip");
}

TEST_F(TokenStreamTest, Numbers) {
  StreamSet streams;
  BuildStream("1 10 0x1 0xAF 0b1 0b0 0b11110000", &streams);
  TokenStream token_stream(*streams.buffered_stream_);
  EXPECT_FALSE(token_stream.IsEOS());

  Token token = token_stream.Get();
  EXPECT_TRUE(Token::NUMBER_DECIMAL == token.type());
  EXPECT_TRUE(token.value() == "1");

  token = token_stream.Get();
  EXPECT_TRUE(Token::NUMBER_DECIMAL == token.type());
  EXPECT_TRUE(token.value() == "10");

  token = token_stream.Get();
  EXPECT_TRUE(Token::NUMBER_HEX == token.type());
  EXPECT_TRUE(token.value() == "0x1");

  token = token_stream.Get();
  EXPECT_TRUE(Token::NUMBER_HEX == token.type());
  EXPECT_TRUE(token.value() == "0xAF");

  token = token_stream.Get();
  EXPECT_TRUE(Token::NUMBER_BINARY == token.type());
  EXPECT_TRUE(token.value() == "0b1");

  token = token_stream.Get();
  EXPECT_TRUE(Token::NUMBER_BINARY == token.type());
  EXPECT_TRUE(token.value() == "0b0");

  token = token_stream.Get();
  EXPECT_TRUE(Token::NUMBER_BINARY == token.type());
  EXPECT_TRUE(token.value() == "0b11110000");
}

TEST_F(TokenStreamTest, InvalidNumbers) {
  StreamSet streams;
  BuildStream("0xX 0b2 0c", &streams);
  TokenStream token_stream(*streams.buffered_stream_);
  EXPECT_FALSE(token_stream.IsEOS());

  Token token = token_stream.Get();
  EXPECT_TRUE(Token::UNKNOWN == token.type());
  EXPECT_TRUE(token.value() == "0");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "xX");

  token = token_stream.Get();
  EXPECT_TRUE(Token::UNKNOWN == token.type());
  EXPECT_TRUE(token.value() == "0");

  token = token_stream.Get();
  EXPECT_TRUE(Token::IDENTIFIER == token.type());
  EXPECT_TRUE(token.value() == "b2");
}

}  // namespace
