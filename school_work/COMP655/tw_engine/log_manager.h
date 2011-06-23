#ifndef INCLUDED_LOG_MANAGER_H__
#define INCLUDED_LOG_MANAGER_H__

#include "logging.h"

#include <vector>

// LogManager is a helper class for maintaing the lifetimes of Logger instances.
class LogManager {
 public:
  // On destruction, free all of the logger instances.
  ~LogManager() {
    std::vector<Logger*>::iterator iter(loggers_.begin()),
      end(loggers_.end());
    for (; iter != end; ++iter) {
      // Free both the inner output stream, and the logger itself.
      delete (*iter)->log_ptr();
      delete *iter;
    }
  }

  void RegisterLogger(Logger* logger) { loggers_.push_back(logger); }
 private:
  std::vector<Logger*> loggers_;
};

#endif  // INCLUDED_LOG_MANAGER_H__
