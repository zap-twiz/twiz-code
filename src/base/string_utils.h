#ifndef INCLUDED_STRING_UTILS_H_
#define INCLUDED_STRING_UTILS_H_

#include <string>

bool IsWhiteSpace(char value);

bool IsLowerCase(char value);
bool IsUpperCase(char value);

char ToLower(char value);
char ToUpper(char value);

bool IsDigit(char value);
bool IsHexDigit(char value);
bool IsBinaryDigit(char value);
bool IsAlphaNumeric(char value);
bool IsAlpha(char value);

bool IsSpecialCharacter(char value);
bool IsValidCharacter(char value);

int HexDigitToInt(char value);
int DecimalDigitToInt(char value);
int BinaryDigitToInt(char value);

// Returns -1 on failure
int HexToInt(std::string const & hex_string);
int DecimalToInt(std::string const & decimal);
int BinaryToInt(std::string const & binary);

#endif  // INCLUDED_STRING_UTILS_H_
