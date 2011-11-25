
#include "parser/parser_utils.h"

#include "parser/parse_error_collection.h"
#include "parser/parse_node.h"

#include "lexer/token.h"
#include "streams/buffered_stream.h"
#include "streams/stream.h"

bool IsTokenIgnored(Token const & token) {
  return Token::COMMENT == token.type();
}

bool IsTokenNumeric(Token const & token) {
  return Token::NUMBER_BINARY == token.type() ||
      Token::NUMBER_DECIMAL == token.type() ||
      Token::NUMBER_HEX == token.type();
}

Token NextToken(Stream<Token>& input_stream) {
  if (input_stream.IsEOS())
    return Token(-1, Token::UNKNOWN, "");

  Token token = input_stream.Get();
  bool found = true;
  while (IsTokenIgnored(token)) {
    if (input_stream.IsEOS()) {
      found = false;
      break;
    }
    token = input_stream.Get();
  }

  if (!found)
    return Token(token.line(), Token::UNKNOWN, "");

  return token;
}

// Returns true if the token was matched, false otherwise
bool ConsumeToken(BufferedTokenStream& input_stream, ParseNode* parse_node,
                  Token::TokenType token_type) {
  Token token = NextToken(input_stream);

  if (Token::UNKNOWN == token.type())
    return false;

  if (token_type == token.type()) {
    parse_node->PushTerminal(token);
    return true;
  }

  // Correct the stream.
  input_stream.Unget(token);
  return false;
}

bool ConsumeToken(BufferedTokenStream& input_stream, ParseNode* parse_node,
                  Token::TokenType* token_types) {
  Token token = NextToken(input_stream);

  if (Token::UNKNOWN == token.type())
    return false;

  for (Token::TokenType* token_type = token_types; *token_type; ++token_type) {
    if (token.type() == *token_type) {
      parse_node->PushTerminal(token);
      return true;
    }
  }

  // Correct the stream.
  input_stream.Unget(token);
  return false;
}

bool ConsumeNonTerminal(BufferedTokenStream& input_stream,
                        ParseNode* parent_node,
                        ParseErrorCollection* error_collection,
                        NonTerminalEvaluator evaluator) {
  ParseNode* result = evaluator(input_stream, error_collection);
  if (result) {
    parent_node->PushNonTerminal(result);
    return true;
  }
  return false;
}

ParseNode* ConsumeNonTerminal(BufferedTokenStream& input_stream,
                              ParseErrorCollection* error_collection,
                              NonTerminalEvaluator* evaluators) {
  ParseErrorCollection local_collection;
  ParseNode* node = NULL;
  for (NonTerminalEvaluator* evaluator = evaluators; *evaluator; ++evaluator) {
    if (node = (*evaluator)(input_stream, &local_collection)) {
      return node;
    }
  }

  // None of the evaluators matched, so return all of the errors.
  error_collection->RegisterErrorCollection(local_collection);
  return NULL;
}

bool ConsumeNonTerminal(BufferedTokenStream& input_stream,
                        ParseNode* parent_node,
                        ParseErrorCollection* error_collection,
                        NonTerminalEvaluator* evaluators) {
  ParseNode* result = ConsumeNonTerminal(input_stream, error_collection,
                                         evaluators);
  if (result) {
    parent_node->PushNonTerminal(result);
    return true;
  }
  return false;
}

void UnwindParseNode(BufferedTokenStream* stream, ParseNode const* parse_node) {
  if (!stream || !parse_node)
    return;

  // Starting at the last element, unget all of the productions in order
  class UnwindVisitor : public ParseNode::Visitor {
   public:
    explicit UnwindVisitor(BufferedTokenStream* stream) : stream_(stream) {}

    virtual void VisitTerminal(Token const & token, size_t offset) {
      stream_->Unget(token);
    }
    virtual void VisitNonTerminal(ParseNode const * parse_node, size_t offset) {
      UnwindParseNode(stream_, parse_node);
    }

   private:
    BufferedTokenStream* stream_;
    DISALLOW_COPY_AND_ASSIGN(UnwindVisitor);
  };

  UnwindVisitor unwind_visitor(stream);
  parse_node->VisitChildrenRightToLeft(&unwind_visitor);
}
