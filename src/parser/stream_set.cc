
#include "parser/stream_set.h"

#include "lexer/line_counting_stream.h"
#include "lexer/token_stream.h"
#include "streams/iostream.h"


StreamSet::~StreamSet() {
  delete buffered_token_stream_;
  delete token_stream_;
  delete buffered_stream_;
  delete io_stream_;
  delete string_stream_;
}

void BuildStream(std::string const & input, StreamSet* stream_set) {
  stream_set->string_stream_ = new std::stringstream(input);
  stream_set->io_stream_ = new IoStream(stream_set->string_stream_);
  stream_set->buffered_stream_ =
      new LineCountingStream(*stream_set->io_stream_);
  stream_set->token_stream_ = new TokenStream(*stream_set->buffered_stream_);
  stream_set->buffered_token_stream_ = new BufferedTokenStream(*stream_set->token_stream_);
}

