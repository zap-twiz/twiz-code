#ifndef INCLUDED_PARSER_PARSER_UTILS_H_
#define INCLUDED_PARSER_PARSER_UTILS_H_

#include "base/base.h"
#include "base/pointer_traits.h"
#include "lexer/token.h"

class Token;
template <class T>
class Stream;

template <class T>
class BufferedStream;
typedef BufferedStream<Token> BufferedTokenStream;

class ParseNode;
class ParseErrorCollection;

bool IsTokenIgnored(Token const & token);
bool IsTokenNumeric(Token const & token);

Token NextToken(Stream<Token>& input_stream);

// Returns true if the token was matched, false otherwise
bool ConsumeToken(BufferedTokenStream& input_stream, ParseNode* parse_node,
                  Token::TokenType token_type);

// Returns true if the token was matched to one of the token_types.
// token_types is delimited by NULL.
bool ConsumeToken(BufferedTokenStream& input_stream, ParseNode* parse_node,
                  Token::TokenType* token_types);

typedef ParseNode* (*NonTerminalEvaluator)(
    BufferedTokenStream&, ParseErrorCollection*);

bool ConsumeNonTerminal(BufferedTokenStream& input_stream,
                        ParseNode* parent_node,
                        ParseErrorCollection* error_collection,
                        NonTerminalEvaluator evaluator);

bool ConsumeNonTerminal(BufferedTokenStream& input_stream,
                        ParseNode* parent_node,
                        ParseErrorCollection* error_collection,
                        NonTerminalEvaluator* evaluators);

ParseNode* ConsumeNonTerminal(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection,
                              NonTerminalEvaluator* evaluators);

void UnwindParseNode(BufferedTokenStream* stream, ParseNode const* parse_node);
class AutoParseNodeUnwinder {
 public:
  AutoParseNodeUnwinder(BufferedTokenStream* stream, ParseNode* parse_node)
      : stream_(stream), parse_node_(parse_node) {}
  ~AutoParseNodeUnwinder() { Unwind(); }

  void Release() { stream_ = NULL; parse_node_ = NULL; }
  void Unwind() {
    if (stream_ && parse_node_)
      UnwindParseNode(stream_, parse_node_);
    Release();
  }

 private:
  BufferedTokenStream* stream_;
  ParseNode* parse_node_;

  DISALLOW_COPY_AND_ASSIGN(AutoParseNodeUnwinder);
};

#endif  // INCLUDED_PARSER_PARSER_UTILS_H_