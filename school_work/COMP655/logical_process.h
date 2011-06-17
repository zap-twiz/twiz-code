#ifndef INCLUDED_LOGICAL_PROCESS_H__
#define INCLUDED_LOGICAL_PROCESS_H__

#include "event.h"

#include <string>
#include <vector>

class NamedEntity {
 public:
  explicit NamedEntity(int id) : id_(id) {}
  virtual ~NamedEntity() {}
  int id() const { return id_; }
  std::string const & name() const { return name_; }

  void set_name(std::string const & name) {
    name_ = name;
  }
 private:
  std::string name_;
  int id_;
};

class ProcessEnvironment;
class State {
 public:
  State(Time time) : logical_time_(time) {}
  virtual ~State() {}

  void set_time(Time time) { logical_time_ = time; }
  Time time() const { return logical_time_; }

 private:
  Time logical_time_;
};

class LogicalProcess : public NamedEntity {
 public:
  LogicalProcess(int id) : NamedEntity(id), local_time_(0) {}
  virtual ~LogicalProcess();

  void ProcessEvent(Event const & event,
                    ProcessEnvironment* process_environment);

  Time LogicalTime() const { return local_time_; }

 protected:
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

  std::vector<Event> processed_events_;

  std::vector<Event> locally_sent_events_;
  
  std::vector< std::vector<Event> > sent_events_;
  std::vector<State*> states_;

  Time local_time_;
};

#endif  // INCLUDED_LOGICAL_PROCESS_H__
