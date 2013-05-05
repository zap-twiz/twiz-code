#ifndef INCLUDED_LEXER_LEX_UTILS_H_
#define INCLUDED_LEXER_LEX_UTILS_H_

#include <string>

template <class T>
class Stream;

template <class T>
class BufferedStream;

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

bool ReadString(BufferedStream<char>& input, std::string* value);

#endif  // INCLUDED_LEXER_LEX_UTILS_H_
