#ifndef INCLUDED_STATE_H__
#define INCLUDED_STATE_H__

#include "event.h"

class State {
 public:
  State(Time time) : logical_time_(time) {}
  virtual ~State() {}

  // Capture the logical time of a LP.
  void set_time(Time time) { logical_time_ = time; }
  Time time() const { return logical_time_; }

 private:
  Time logical_time_;
};

#endif  // INCLUDED_STATE_H__
