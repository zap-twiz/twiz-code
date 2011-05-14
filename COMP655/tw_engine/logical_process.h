#ifndef INCLUDED_LOGICAL_PROCESS_H__
#define INCLUDED_LOGICAL_PROCESS_H__

#include "event.h"
#include "logging.h"
#include "named_entity.h"
#include "state.h"

#include <vector>

class ProcessEnvironment;

class LogicalProcess : public NamedEntity {
 public:
  LogicalProcess(int id) : NamedEntity(id), local_time_(0), logger_(NULL) {}
  virtual ~LogicalProcess();

  void set_logger(Logger* logger) { logger_ = logger; }
  std::ostream& log() const;

  void ReceiveEvent(Event const & event);
  void EvaluateInputQueue(ProcessEnvironment* process_environment);

  // The time at which the LP last received an event.
  Time LogicalTime() const { return local_time_; }

  Time LocalVirtualTime() const;
  void FossilCollect(Time gvt);

 protected:
  // Returns true if the event was processed, false if it needs to be deferred
  bool ProcessEvent(Event const & event,
                    ProcessEnvironment* process_environment);
  bool ProcessFutureEvent(Event const & event,
                          ProcessEnvironment* process_environment);
  bool ProcessHistoricalEvent(Event const & event,
                              ProcessEnvironment* process_environment);

  void Rollback(Time time, std::vector<Event>* to_re_evaluate,
                ProcessEnvironment* process_environment);

  void PostProcessEvent(Event const& event);
  void SendEvent(Event const& event,
                 ProcessEnvironment* process_environment);

  virtual State* BuildMemento() = 0;
  virtual void ResurrectMemento(State* state) {
    local_time_ = state->time();
  };

  virtual Time Evaluate(Event const & event,
                        ProcessEnvironment* process_environment) = 0;

  void ProcessStraggler(Event const & straggler,
                        ProcessEnvironment* process_environment);

  void ProcessAntimessage(Event const & anti_event,
                          ProcessEnvironment* process_environment);

  void PushState(State* state) { states_.push_back(state); }

private:
  void ResolveInputQueue(std::vector<Event>* input_queue);

  std::vector<Event> input_queue_;

  std::vector<Event> processed_events_;
  std::vector<State*> states_;

  std::vector<Event> locally_sent_events_;
  std::vector< std::vector<Event> > sent_events_;

  Time local_time_;

  Logger* logger_;
};

#endif  // INCLUDED_LOGICAL_PROCESS_H__
