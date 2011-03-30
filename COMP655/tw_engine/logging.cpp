
#include <Windows.h>

#include "logging.h"

#include <assert.h>
#include <fstream>

std::ostream* InitOutputStream(char const* name) {
  std::ostream* output_stream = new std::fstream(name,
      (std::ios_base::out | std::ios_base::trunc));
  assert(output_stream->good());
  return output_stream;
}

std::ostream& Logger::log() {
  LARGE_INTEGER current_ticks = {};
  QueryPerformanceCounter(&current_ticks);

  return *output_stream_ << std::endl << current_ticks.QuadPart << ": ";
}
