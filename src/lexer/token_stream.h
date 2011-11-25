#ifndef INCLUDED_LEXER_TOKEN_STREAM_H_
#define INCLUDED_LEXER_TOKEN_STREAM_H_

#include "lexer/token.h"
#include "streams/stream.h"

class LineCountingStream;

class TokenStream : public Stream<Token> {
 public:
  typedef LineCountingStream InputStream;

  TokenStream(InputStream& input_stream)
    : input_stream_(input_stream) {}

  size_t CurrentLine() const;

  // NOT IMPLEMENTED
  virtual size_t GetN(Token* data, size_t count) { return 0; }

  virtual Token Get();
  virtual bool IsEOS() const;

 private:
  InputStream& input_stream_;
};

#endif // INCLUDED_LEXER_TOKEN_STREAM_H_
