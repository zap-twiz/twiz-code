#ifndef INCLUDED_PROCESS_ENVIRONMENT_H__
#define INCLUDED_PROCESS_ENVIRONMENT_H__

#include <vector>

#include "event_queue.h"

class LogicalProcess;

class ProcessEnvironment {
 public:
  ~ProcessEnvironment();

  void RegisterLogicalProcess(LogicalProcess* process) {
    processes_.push_back(process);
  }

  LogicalProcess* process(int id) { return processes_[id]; }

  bool Idle() const { return event_queue().empty(); }

  int next_id() const { return processes_.size(); }
  void Send(Event const& event);

  Time VirtualTime() const;

 public:
  EventQueue& event_queue() { return message_queue_; }
  EventQueue const & event_queue() const { return message_queue_; }

 private:
  typedef std::vector<LogicalProcess*> ProcessVector;
  ProcessVector processes_;

  EventQueue message_queue_;
};

#endif  // INCLUDED_PROCESS_ENVIRONMENT_H__
