#ifndef INCLUDED_STATE_H__
#define INCLUDED_STATE_H__

#include "event.h"

// State is the basic interface for management of LP internals during
// roll-back processing.  States, and subclasses of states are used
// as mementos to store all relevant information to restore an LP
// to a particular state in time.
class State {
 public:
  State(Time time) : logical_time_(time) {}
  virtual ~State() {}

  // The base State class only stores the time of the LP.
  void set_time(Time time) { logical_time_ = time; }
  Time time() const { return logical_time_; }

 private:
  Time logical_time_;
};

#endif  // INCLUDED_STATE_H__
