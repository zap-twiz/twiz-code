#ifndef INCLUDED_LOGGING_H_
#define INCLUDED_LOGGING_H_

#include <ostream>

class Logger {
 public:
  Logger(std::ostream* output_stream) : output_stream_(output_stream) {}
  std::ostream& log();
  std::ostream* log_ptr() { return output_stream_; }
 private:
  std::ostream* output_stream_;
};

std::ostream* InitOutputStream(char const* name);
#endif
