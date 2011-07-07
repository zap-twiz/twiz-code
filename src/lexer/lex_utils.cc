
#include "lexer/lex_utils.h"

#include "streams/buffered_stream.h"
#include "streams/stream.h"


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

std::string ReadNonWhiteSpace(BufferedStream<char>& input) {
  std::string token;
  while (!input.IsEOS()) {
    char value = input.Get();
    if (IsWhiteSpace(value)) {
      input.Unget(value);
      break;
    }
    token += value;
  }
  return token;
}

void SkipWhiteSpace(BufferedStream<char>& input) {
  while (!input.IsEOS()) {
    char value = input.Get();
    if (!IsWhiteSpace(value)) {
      input.Unget(value);
      break;
    }
  }
}

void ReadLine(Stream<char>& input) {
  while (!input.IsEOS()) {
    if (input.Get() == '\n')
      break;
  }
}

bool ReadCPPCommentTail(BufferedStream<char>& input) {
  std::vector<char> read_characters;
  while (!input.IsEOS()) {
    char value = input.Get();
    read_characters.push_back(value);
    if (value == '*') {
      if (!input.IsEOS()) {
        value = input.Get();
        read_characters.push_back(value);
        if (value == '/')
          return true;
      }
    }
  }

  std::vector<char>::reverse_iterator riter(read_characters.rbegin()),
    rend(read_characters.rend());
  for ( ; riter != rend; ++riter)
    input.Unget(*riter);

  return false;
}

bool ReadBinaryNumber(BufferedStream<char>& input, std::string* value) {
  value->clear();
  if (input.IsEOS())
    return false;

  char read = input.Get();
  if (read != '0') {
    input.Unget(read);
    return false;
  }

  read = input.Get();
  if (read != 'b') {
    input.Unget(read);
    input.Unget('0');
    return false;
  }

  std::string number("0b");
  bool is_number = false;
  while (!input.IsEOS()) {
    read = input.Get();
    if (!IsBinaryDigit(read)) {
      input.Unget(read);
      break;
    } else {
      number += read;
      is_number = true;
    }
  }

  if (!is_number) {
    int offset = number.size() - 1;
    for (; offset >= 0; --offset)
      input.Unget(number[offset]);
  } else {
    *value = number;
  }

  return is_number;
}

bool ReadHexNumber(BufferedStream<char>& input, std::string* value) {
  value->clear();
  if (input.IsEOS())
    return false;

  char read = input.Get();
  if (read != '0') {
    input.Unget(read);
    return false;
  }

  read = input.Get();
  if (read != 'x') {
    input.Unget(read);
    input.Unget('0');
    return false;
  }

  std::string number("0x");
  bool is_number = false;
  while (!input.IsEOS()) {
    read = input.Get();
    if (!IsHexDigit(read)) {
      input.Unget(read);
      break;
    } else {
      number += read;
      is_number = true;
    }
  }

  if (!is_number) {
    int offset = number.size() - 1;
    for (; offset >= 0; --offset)
      input.Unget(number[offset]);
  } else {
    *value = number;
  }

  return is_number;
}

bool ReadDecimalNumber(BufferedStream<char>& input, std::string* value) {
  value->clear();
  if (input.IsEOS())
    return false;

  bool is_number = false;
  std::string number;
  while (!input.IsEOS()) {
    char read = input.Get();
    if (!IsDigit(read)) {
      input.Unget(read);
      break;
    } else {
      number += read;
      is_number = true;
    }
  }

  if (!is_number) {
   int offset = number.size() - 1;
    for (; offset >= 0; --offset)
      input.Unget(number[offset]);
  } else {
    *value = number;
  }

  return is_number;
}

bool ReadNumber(BufferedStream<char>& input, std::string* value) {
  if (ReadHexNumber(input, value))
    return true;

  if (ReadBinaryNumber(input, value))
    return true;

  if (ReadDecimalNumber(input, value))
    return true;

  return false;
}

bool ReadIdentifierTail(BufferedStream<char>& input, std::string* value) {
  value->clear();
  bool identifier_read = false;
  while (!input.IsEOS()) {
    char read = input.Get();
    if (IsAlphaNumeric(read) || '_' == read) {
      *value += read;
      identifier_read = true;
    } else {
      input.Unget(read);
      break;
    }
  }
  return identifier_read;
}

bool ReadIdentifier(BufferedStream<char>& input, std::string* value) {
  value->clear();
  if (input.IsEOS())
    return false;

  char read = input.Get();
  if (IsAlpha(read) || '_' == read) {
    *value += read;
    std::string tail;
    if (ReadIdentifierTail(input, &tail))
      *value += tail;
    return true;
  } else {
    input.Unget(read);
  }

  return false;
}

bool ReadString(BufferedStream<char>& input, std::string* value) {
  value->clear();

  if (input.IsEOS())
    return false;

  char read = input.Get();
  if ('"' != read) {
    input.Unget(read);
    return false;
  }

  bool string_read = false;
  while (!input.IsEOS()) {
    read = input.Get();
    if ('"' == read) {
      string_read = true;
      break;
    }
    *value += read;
  }

  if (!string_read) {
    int offset = value->size() - 1;
    for (; offset >= 0; --offset)
      input.Unget((*value)[offset]);
    value->clear();

    input.Unget('"');
  }

  return string_read;
}
