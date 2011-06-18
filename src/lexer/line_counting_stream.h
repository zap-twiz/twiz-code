#ifndef INCLUDED_LEXER_LINE_COUNTING_STREAM_H_
#define INCLUDED_LEXER_LINE_COUNTING_STREAM_H_

#include "streams/counter.h"
#include "streams/stream_with_observer.h"

class LineCountingStream : public ObservedStream<char> ,
                           public CountObserver<char> {
 public:
   LineCountingStream(BaseStream& input_stream)
       : ObservedStream<char>(input_stream), CountObserver<char>('\n') {
     set_observer(this);
     SetCount(1);  // File lines start at 1!
   }
};

#endif  // INCLUDED_LEXER_LINE_COUNTING_STREAM_H_
