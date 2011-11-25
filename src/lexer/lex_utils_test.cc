#include "gtest/gtest.h"

#include "lexer/lex_utils.h"
#include "streams/buffered_stream.h"
#include "streams/iostream.h"

namespace {

class LexUtilsTest : public ::testing::Test {
};

TEST_F(LexUtilsTest, WhiteSpace) {
  static const char kWhiteSpace[] = {' ', '\t', '\r', '\n'};
  for (int x = 0; x < 4; ++x ) {
    EXPECT_TRUE(IsWhiteSpace(kWhiteSpace[x]));
  }

  EXPECT_FALSE(IsWhiteSpace('a'));
}

TEST_F(LexUtilsTest, Case) {
  for (char letter = 'a'; letter <= 'z'; ++letter) {
    EXPECT_TRUE(IsLowerCase(letter));
    EXPECT_FALSE(IsUpperCase(letter));

    EXPECT_TRUE(IsUpperCase(ToUpper(letter)));
  }

  for (char letter = 'A'; letter <= 'Z'; ++letter) {
    EXPECT_TRUE(IsUpperCase(letter));
    EXPECT_FALSE(IsLowerCase(letter));

    EXPECT_TRUE(IsLowerCase(ToLower(letter)));
  }
}

TEST_F(LexUtilsTest, AlphaNumeric) {
  for (unsigned char x = 0; ; ++x) {
    if (IsDigit(x))
      EXPECT_FALSE(IsAlpha(x));

    if (IsAlpha(x))
      EXPECT_FALSE(IsDigit(x));

    if (IsWhiteSpace(x))
      EXPECT_FALSE(IsAlpha(x) || IsDigit(x) || IsSpecialCharacter(x));

    if (0xFF == x)
      break;
  }

  for (unsigned char x = 'A'; x <= 'F'; ++x)
    EXPECT_TRUE(IsHexDigit(x));

  EXPECT_TRUE(IsSpecialCharacter('<'));
  EXPECT_TRUE(IsSpecialCharacter('>'));
  EXPECT_TRUE(IsSpecialCharacter('-'));
  EXPECT_TRUE(IsSpecialCharacter('.'));
  EXPECT_TRUE(IsSpecialCharacter('['));
  EXPECT_TRUE(IsSpecialCharacter(']'));
  EXPECT_TRUE(IsSpecialCharacter('('));
  EXPECT_TRUE(IsSpecialCharacter(')'));
  EXPECT_TRUE(IsSpecialCharacter('{'));
  EXPECT_TRUE(IsSpecialCharacter('}'));
  EXPECT_TRUE(IsSpecialCharacter(','));
  EXPECT_TRUE(IsSpecialCharacter(';'));

  for (unsigned char x = 0; ; ++x){
    if (IsValidCharacter(x)) {
      EXPECT_TRUE(IsSpecialCharacter(x) || IsAlpha(x) || IsDigit(x) ||
                  IsAlphaNumeric(x));
      EXPECT_FALSE(IsWhiteSpace(x));
    }

    if (0xFF == x)
      break;
  }
}


TEST_F(LexUtilsTest, ReadNonWhiteSpace) {
  const std::string value("This is\t\r\na_test.");
  std::stringstream string_stream(value);
  IoStream io_stream(&string_stream);
  BufferedStream<char> buffered_stream(io_stream);

  std::string non_ws = ReadNonWhiteSpace(buffered_stream);
  EXPECT_TRUE(non_ws == "This");
  EXPECT_TRUE(' ' == buffered_stream.Get());

  non_ws = ReadNonWhiteSpace(buffered_stream);
  EXPECT_TRUE(non_ws == "is");
  EXPECT_TRUE('\t' == buffered_stream.Get());
  EXPECT_TRUE('\r' == buffered_stream.Get());
  EXPECT_TRUE('\n' == buffered_stream.Get());

  non_ws = ReadNonWhiteSpace(buffered_stream);
  EXPECT_TRUE(non_ws == "a_test.");

  EXPECT_TRUE(buffered_stream.IsEOS());
}

TEST_F(LexUtilsTest, SkipWhiteSpace) {
  const std::string value("\n\r\t X");
  std::stringstream string_stream(value);
  IoStream io_stream(&string_stream);
  BufferedStream<char> buffered_stream(io_stream);

  SkipWhiteSpace(buffered_stream);
  EXPECT_TRUE('X' == buffered_stream.Get());
}

TEST_F(LexUtilsTest, ReadLine) {
  const std::string value("1\n2\n\n3");
  std::stringstream string_stream(value);
  IoStream io_stream(&string_stream);
  NestedStream<char> stream(io_stream);

  ReadLine(stream);
  EXPECT_TRUE(stream.Get() == '2');
  ReadLine(stream);
  EXPECT_TRUE(stream.Get() == '\n');
  ReadLine(stream);
  EXPECT_TRUE(stream.IsEOS());
}

TEST_F(LexUtilsTest, ReadCPPCommentTail) {
  {
    const std::string value("This is a comment tail. */");
    std::stringstream string_stream(value);
    IoStream io_stream(&string_stream);
    BufferedStream<char> buffered_stream(io_stream);

    EXPECT_TRUE(ReadCPPCommentTail(buffered_stream));
    EXPECT_TRUE(buffered_stream.IsEOS());
  }

  {
    const std::string value("This is not a comment tail. * /");
    std::stringstream string_stream(value);
    IoStream io_stream(&string_stream);
    BufferedStream<char> buffered_stream(io_stream);

    EXPECT_FALSE(ReadCPPCommentTail(buffered_stream));
    EXPECT_FALSE(buffered_stream.IsEOS());
    EXPECT_TRUE('T' == buffered_stream.Get());
  }
}

TEST_F(LexUtilsTest, ReadNumber) {
  const std::string value("A1\n100\n0300\n0x400\n0xAFF\n0b1\n0b0\n0xnot");
  std::stringstream string_stream(value);
  IoStream io_stream(&string_stream);
  BufferedStream<char> buffered_stream(io_stream);

  std::string number_value;
  EXPECT_FALSE(ReadNumber(buffered_stream, &number_value));

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadNumber(buffered_stream, &number_value));
  EXPECT_TRUE(number_value == "100");

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadNumber(buffered_stream, &number_value));
  EXPECT_TRUE(number_value == "0300");

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadNumber(buffered_stream, &number_value));
  EXPECT_TRUE(number_value == "0x400");

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadNumber(buffered_stream, &number_value));
  EXPECT_TRUE(number_value == "0xAFF");

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadNumber(buffered_stream, &number_value));
  EXPECT_TRUE(number_value == "0b1");

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadNumber(buffered_stream, &number_value));
  EXPECT_TRUE(number_value == "0b0");

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadNumber(buffered_stream, &number_value));
  EXPECT_TRUE(number_value == "0");

  EXPECT_FALSE(ReadNumber(buffered_stream, &number_value));
  EXPECT_TRUE(number_value.empty());
}

TEST_F(LexUtilsTest, ReadIdentifier) {
  const std::string value("ident\nident1\n_ident\n[ident\n0ident");
  std::stringstream string_stream(value);
  IoStream io_stream(&string_stream);
  BufferedStream<char> buffered_stream(io_stream);

  std::string ident;
  EXPECT_TRUE(ReadIdentifier(buffered_stream, &ident));
  EXPECT_STREQ("ident", ident.c_str());

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadIdentifier(buffered_stream, &ident));
  EXPECT_STREQ("ident1", ident.c_str());

  ReadLine(buffered_stream);
  EXPECT_TRUE(ReadIdentifier(buffered_stream, &ident));
  EXPECT_STREQ("_ident", ident.c_str());

  ReadLine(buffered_stream);
  EXPECT_FALSE(ReadIdentifier(buffered_stream, &ident));
  EXPECT_TRUE(ident.empty());
  EXPECT_TRUE('[' == buffered_stream.Get());
  EXPECT_TRUE(ReadIdentifier(buffered_stream, &ident));
  EXPECT_STREQ("ident", ident.c_str());

  ReadLine(buffered_stream);
  EXPECT_FALSE(ReadIdentifier(buffered_stream, &ident));
  EXPECT_TRUE('0' == buffered_stream.Get());
  EXPECT_TRUE(ReadIdentifier(buffered_stream, &ident));
  EXPECT_STREQ("ident", ident.c_str());
}

TEST_F(LexUtilsTest, ReadString) {
  const std::string value("\"string1\"\"not_a_string");
  std::stringstream string_stream(value);
  IoStream io_stream(&string_stream);
  BufferedStream<char> buffered_stream(io_stream);

  std::string string;
  EXPECT_TRUE(ReadString(buffered_stream, &string));
  EXPECT_STREQ("string1", string.c_str());

  EXPECT_FALSE(ReadString(buffered_stream, &string));
  EXPECT_TRUE('"' == buffered_stream.Get());
}

}  // namespace
