#ifndef INCLUDED_PARSER_PARSER_UTILS_H_
#define INCLUDED_PARSER_PARSER_UTILS_H_


class Token;
template <class T>
class Stream;

class ParseNode;

bool IsTokenIgnored(Token const & token);
bool IsTokenNumeric(Token const & token);

Token NextToken(Stream<Token>& input_stream);

#endif  // INCLUDED_PARSER_PARSER_UTILS_H_