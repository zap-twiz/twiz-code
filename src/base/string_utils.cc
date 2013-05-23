
#include "base/string_utils.h"

#include <sstream>

#include <assert.h>

namespace {
const char kCaseOffset = 'A' - 'a';
}

bool IsWhiteSpace(char value) {
  switch(value) {
    case ' ':
    case '\n':
    case '\t':
    case '\r':
      return true;
    default:
      return false;
  }
}

bool IsLowerCase(char value) {
  return value >= 'a' && value <= 'z';
}

bool IsUpperCase(char value) {
  return value >= 'A' && value <= 'Z';
}

char ToLower(char value) {
  if (IsUpperCase(value))
    value -= kCaseOffset;
  return value;
}

char ToUpper(char value) {
  if (IsLowerCase(value))
    value += kCaseOffset;
  return value;
}

bool IsDigit(char value) {
  return value >= '0' && value <= '9';
}

bool IsHexDigit(char value) {
  return IsDigit(value) || (value >= 'A' && value <= 'F');
}

bool IsBinaryDigit(char value) {
  return '0' == value || '1' == value;
}

bool IsAlphaNumeric(char value) {
  return IsDigit(value) || IsLowerCase(value) || IsUpperCase(value);
}

bool IsAlpha(char value) {
  return IsLowerCase(value) || IsUpperCase(value);
}

bool IsSpecialCharacter(char value) {
  switch(value) {
    case '<':
    case '>':
    case '-':
    case '.':
    case '[': case ']':
    case '(': case ')':
    case '{': case '}':
    case ',':
    case ';':
      return true;
    default:
      return false;
  }
}

bool IsValidCharacter(char value) {
  return IsAlphaNumeric(value) || IsSpecialCharacter(value);
}

int HexDigitToInt(char value) {
  assert(IsHexDigit(value));
  if (IsDigit(value)) {
    return DecimalDigitToInt(value);
  }
  return value - 'A' + 10;
}

int DecimalDigitToInt(char value) {
  assert(IsDigit(value));
  return value - '0';
}

int BinaryDigitToInt(char value) {
  assert(IsBinaryDigit(value));
  return value - '0';
}

int HexToInt(std::string const & hex_string) {
  std::stringstream string_stream(hex_string);

  char temp[2];
  string_stream.read(&temp[0], 2);
  if ('0' != temp[0] || 'x' != temp[1]) {
    return -1;
  }

  int value = 0;
  while (!string_stream.eof()) {
    char data;
    string_stream.read(&data, 1);

    if (string_stream.eof())
      break;

    if (!IsHexDigit(data)) {
      return -1;
    }
    value = value * 16 + HexDigitToInt(data);
  }
  return value;
}

int DecimalToInt(std::string const & decimal_string) {
  std::stringstream string_stream(decimal_string);

  int value = 0;
  while (!string_stream.eof()) {
    char data;
    string_stream.read(&data, 1);

    if (string_stream.eof())
      break;

    if (!IsDigit(data)) {
      return -1;
    }
    value = value * 10 + HexDigitToInt(data);
  }
  return value;
}

int BinaryToInt(std::string const & binary_string) {
  std::stringstream string_stream(binary_string);

  char temp[2];
  string_stream.read(&temp[0], 2);
  if ('0' != temp[0] || 'b' != temp[1]) {
    return -1;
  }

  int value = 0;
  while (!string_stream.eof()) {
    char data;
    string_stream.read(&data, 1);

    if (string_stream.eof())
      break;

    if (!IsBinaryDigit(data)) {
      return -1;
    }
    value = value * 2 + HexDigitToInt(data);
  }
  return value;
}
