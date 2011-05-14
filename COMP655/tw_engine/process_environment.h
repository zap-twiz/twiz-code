#ifndef INCLUDED_PROCESS_ENVIRONMENT_H__
#define INCLUDED_PROCESS_ENVIRONMENT_H__

#include "event_queue.h"
#include "post_master.h"

#include <map>

class LogicalProcess;

class ProcessEnvironment {
 public:
  ProcessEnvironment() : post_master_(NULL) {}
  ~ProcessEnvironment();

  void set_postmaster(PostMaster* post_master) {
    post_master_ = post_master;
  }

  void RegisterLogicalProcess(LogicalProcess* process);
  LogicalProcess* process(int id);

  bool IsIdle() const { return event_queue().empty(); }
  void Send(Event const& event);

  Time VirtualTime() const;

  void FossilCollect(Time gvt);

 public:
  EventQueue& event_queue() { return message_queue_; }
  EventQueue const & event_queue() const { return message_queue_; }

 private:
  typedef std::map<int, LogicalProcess*> ProcessMap;
  ProcessMap processes_;

  EventQueue message_queue_;

  PostMaster* post_master_;
};

#endif  // INCLUDED_PROCESS_ENVIRONMENT_H__
