#ifndef INCLUDED_LEXER_LEX_UTILS_H_
#define INCLUDED_LEXER_LEX_UTILS_H_

#include <string>

template <class T>
class Stream;

template <class T>
class BufferedStream;

bool IsWhiteSpace(char value);

bool IsLowerCase(char value);
bool IsUpperCase(char value);

char ToLower(char value);
char ToUpper(char value);

bool IsDigit(char value);
bool IsHexDigit(char value);
bool IsAlphaNumeric(char value);
bool IsAlpha(char value);

bool IsSpecialCharacter(char value);
bool IsValidCharacter(char value);

std::string ReadNonWhiteSpace(BufferedStream<char>& input);
void SkipWhiteSpace(BufferedStream<char>& input);

void ReadLine(Stream<char>& input);

bool ReadCPPCommentTail(BufferedStream<char>& input);

bool ReadHexNumber(BufferedStream<char>& input, std::string* value);
bool ReadDecimalNumber(BufferedStream<char>& input, std::string* value);
bool ReadBinaryNumber(BufferedStream<char>& input, std::string* value);
bool ReadNumber(BufferedStream<char>& input, std::string* value);

bool ReadIdentifierTail(BufferedStream<char>& input, std::string* value);
bool ReadIdentifier(BufferedStream<char>& input, std::string* value);

#endif  // INCLUDED_LEXER_LEX_UTILS_H_
