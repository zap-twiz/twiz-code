#ifndef INCLUDED_LOG_MANAGER_H__
#define INCLUDED_LOG_MANAGER_H__

#include "logging.h"

#include <vector>

class LogManager {
 public:
  ~LogManager() {
    std::vector<Logger*>::iterator iter(loggers_.begin()),
      end(loggers_.end());
    for (; iter != end; ++iter) {
      delete (*iter)->log_ptr();
      delete *iter;
    }
  }

  void RegisterLogger(Logger* logger) { loggers_.push_back(logger); }
 private:
  std::vector<Logger*> loggers_;
};

#endif  // INCLUDED_LOG_MANAGER_H__
