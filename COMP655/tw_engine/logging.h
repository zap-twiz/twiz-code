#ifndef INCLUDED_LOGGING_H_
#define INCLUDED_LOGGING_H_

#include <ostream>

// Logger provides capabilities for the system to write log files.
// A Logger is bound with each LP, which can then write individual
// status logs.
class Logger {
 public:
  // Construct with an iostream object, to which the output will be directed.
  Logger(std::ostream* output_stream) : output_stream_(output_stream) {}

  std::ostream& log();

  std::ostream* log_ptr() { return output_stream_; }
 private:
  std::ostream* output_stream_;
};

// Open a file with name |name| for writing as an ostream.
std::ostream* InitOutputStream(char const* name);
#endif
