#include "gtest/gtest.h"

#include "base/string_utils.h"

class StringUtilsTest : public ::testing::Test {
};

TEST_F(StringUtilsTest, WhiteSpace) {
  static const char kWhiteSpace[] = {' ', '\t', '\r', '\n'};
  for (int x = 0; x < 4; ++x ) {
    EXPECT_TRUE(IsWhiteSpace(kWhiteSpace[x]));
  }

  EXPECT_FALSE(IsWhiteSpace('a'));
}

TEST_F(StringUtilsTest, Case) {
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

TEST_F(StringUtilsTest, AlphaNumeric) {
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

TEST_F(StringUtilsTest, HexToInt) {
  EXPECT_EQ(HexToInt("0x0"), 0);
  EXPECT_EQ(HexToInt("0x1"), 1);
  EXPECT_EQ(HexToInt("0xA"), 10);
  EXPECT_EQ(HexToInt("0xF"), 15);
  EXPECT_EQ(HexToInt("0x100"), 256);

  EXPECT_EQ(HexToInt("F"), -1);
  EXPECT_EQ(HexToInt("0XA"), -1);
}

TEST_F(StringUtilsTest, DecimalToInt) {
  EXPECT_EQ(DecimalToInt("0"), 0);
  EXPECT_EQ(DecimalToInt("5"), 5);
  EXPECT_EQ(DecimalToInt("10"), 10);
  EXPECT_EQ(DecimalToInt("101"), 101);
}

TEST_F(StringUtilsTest, BinaryToInt) {
  EXPECT_EQ(BinaryToInt("0b0"), 0);
  EXPECT_EQ(BinaryToInt("0b1"), 1);
  EXPECT_EQ(BinaryToInt("0b1111"), 15);
}

