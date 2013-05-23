#ifndef INCLUDED_PARSER_STREAM_SET_H_
#define INCLUDED_PARSER_STREAM_SET_H_

#include <sstream>

#include "lexer/token_stream.h"

class IoStream;
class LineCountingStream;
class TokenStream;

struct StreamSet {
  std::stringstream* string_stream_;
  IoStream* io_stream_;
  LineCountingStream* buffered_stream_;
  TokenStream* token_stream_;
  BufferedTokenStream* buffered_token_stream_;

  ~StreamSet();
};

// For a given input string, build a stream set that can process it as a buffered
// stream.
void BuildStream(std::string const & input, StreamSet* stream_set);

#endif
