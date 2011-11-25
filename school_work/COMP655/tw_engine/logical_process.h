#ifndef INCLUDED_LOGICAL_PROCESS_H__
#define INCLUDED_LOGICAL_PROCESS_H__

#include "event.h"
#include "logging.h"
#include "named_entity.h"
#include "state.h"

#include <vector>

class ProcessEnvironment;

// Class implementing the logical process (LP) abstraction of the time-warp
// system.  LogicalProcesses are named, and have an associated id for look-up
// when sending messages.
class LogicalProcess : public NamedEntity {
 public:
  LogicalProcess(int id) : NamedEntity(id), local_time_(0), logger_(NULL) {}
  virtual ~LogicalProcess();

  // Bind the logger that will be used to output traces for this LP.
  void set_logger(Logger* logger) { logger_ = logger; }
  std::ostream& log() const;

  // Register an event to the input queue of the LP.  The event is not
  // processed.
  void ReceiveEvent(Event const & event);

  // For all the events registered in the input queue, evaluate them.
  void EvaluateInputQueue(ProcessEnvironment* process_environment);

  // The time at which the LP last received an event.
  Time LogicalTime() const { return local_time_; }

  Time LocalVirtualTime() const;

  // Perform fossil collection on the LP.  Free any allocated resources
  // corresponding to events processed prior to |gvt|.
  void FossilCollect(Time gvt);

 protected:
  // Returns true if the event was processed, false if it needs to be deferred.
  bool ProcessEvent(Event const & event,
                    ProcessEnvironment* process_environment);

  // Process an event that arrived from the future.  This is the normal path,
  // through which most events will be routed.
  bool ProcessFutureEvent(Event const & event,
                          ProcessEnvironment* process_environment);

  // Process a straggler event.  This can either be a late straggler, or an
  // anti message.  The LP is rolled back, and anti-messages are produced.
  bool ProcessHistoricalEvent(Event const & event,
                              ProcessEnvironment* process_environment);

  // Roll back the LP to |time|.  |to_re_evaluate| is the set of events
  // that requires re-processing.
  void Rollback(Time time, std::vector<Event>* to_re_evaluate,
                ProcessEnvironment* process_environment);

  // Called upon completion of event processing.  Finished the packaging of
  // memento information used to store the details of the processing of this
  // event - messages sent, and internal state.
  void PostProcessEvent(Event const& event);

  // Send an event to another LP in the system.  The event will be noted
  // in the LP output queue.
  void SendEvent(Event const& event,
                 ProcessEnvironment* process_environment);

  // Virtual interface used to construct state mementos for subclsses
  // of LPs.
  virtual State* BuildMemento() = 0;

  // Interface method used to resurrect the internal state of the LP
  // given a memento.
  virtual void ResurrectMemento(State* state) {
    local_time_ = state->time();
  };

  // Core routine to evaluate an event.  To be implemented by subclasses.
  virtual Time Evaluate(Event const & event,
                        ProcessEnvironment* process_environment) = 0;

  void PushState(State* state) { states_.push_back(state); }

private:
  // Helper routine that performs anti-message annihilation on events
  // in the input queue.
  void ResolveInputQueue(std::vector<Event>* input_queue);

  // The input queue for this LP.  Only messages sharing the same-time stamp,
  // and unpaired anti-messages (when the positive message has yet to be
  // receive) are stored here.
  std::vector<Event> input_queue_;

  // State management that is referenced during roll-back processing.
  // |processed_events_| is the vector of all events processed by the lp.
  std::vector<Event> processed_events_;

  // Array of all state mementos corresponding the the internal state of
  // the lp when processing the events in |processed_events_|.
  std::vector<State*> states_;

  // Array of events sent during an individual event processing call.
  // Used to package all of the events sent in response to an input
  // event for storage on the output queue.
  std::vector<Event> locally_sent_events_;

  // The output queue of the LP.  An array of arrays of all events
  // sent for each of the events stored in |processed_events_|.
  // All of the events stored are the negative versions of the events
  // sent during processing.
  std::vector< std::vector<Event> > sent_events_;

  // The time stamp of the lp.  Equal to the receive time-stamp of the
  // last evaluatd event.
  Time local_time_;

  Logger* logger_;
};

#endif  // INCLUDED_LOGICAL_PROCESS_H__
