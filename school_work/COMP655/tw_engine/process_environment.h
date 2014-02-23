#ifndef INCLUDED_PROCESS_ENVIRONMENT_H__
#define INCLUDED_PROCESS_ENVIRONMENT_H__

#include "event_queue.h"
#include "post_master.h"

#include <map>

class LogicalProcess;

// The ProcessEnvironment class provides the environment in which LPs live,
// and communicate with the simulation as a whole.  A ProcessEnvironment
// controls the lifetime of the registered LPs.  All message traffic during
// event evaluation is passed through the environment.
// An event queue is maintained for all events sent to the registered LPs.
class ProcessEnvironment {
 public:
  ProcessEnvironment() : post_master_(NULL) {}
  ~ProcessEnvironment();

  // Assign the post-master for the environment.
  // Note:  The ProcessEnvironment does not own the |post_master| instance.
  void set_postmaster(PostMaster* post_master) {
    post_master_ = post_master;
  }

  // Registration function to bind |process| to this environment.
  void RegisterLogicalProcess(LogicalProcess* process);

  // Accessor function.  Returns the LogicalProcess instance with identifier,
  // |id|.
  LogicalProcess* process(int id);

  // Function used to detect if the ProcessEnvironment has no remaining internal
  // work.
  bool IsIdle() const { return event_queue().empty(); }

  // Function to send |event|.  All LPs invoke this routine to send messages.
  // Redirects to the bound post-master.
  void Send(Event const& event);

  Time VirtualTime() const;

  // Perform fossil-collection on all resources allocated to all LPs in the
  // environment.
  void FossilCollect(Time gvt);

  // Accessors for the event queue associated
  EventQueue& event_queue() { return message_queue_; }
  EventQueue const & event_queue() const { return message_queue_; }

 private:
  // Processes are stored in a map from their id to their instance.
  // TODO:  Use a faster look-up structure.
  typedef std::map<int, LogicalProcess*> ProcessMap;
  ProcessMap processes_;

  // The event queue for all events sent to registered LPs.
  EventQueue message_queue_;

  PostMaster* post_master_;
};

#endif  // INCLUDED_PROCESS_ENVIRONMENT_H__
