

#include "stdafx.h"

#include "simulation_engine.h"
#include "logical_process.h"

#include <assert.h>
#include <set>

void SimulationEngine::TimeStep() {
  // Check to see if there are ack-messages waiting
  post_master_->ResolveAckMessages();

  if (post_master_->ReceiveGVTRequest()) {
    post_master_->SendGVTResponse(LocalVirtualTime());
  }

  Time gvt;
  if (post_master_->ReceiveGVTValue(&gvt)) {
    // fossil collect
    ReceiveGlobalVirtualTime(gvt);
  }

  if (env_.IsIdle()) {
    // If there are no local events to process, try to extract
    // some from the post-master.
    Event remote_event;
    if (post_master_->ReceiveMessage(&remote_event)) {
      env_.Send(remote_event);
    }
    return;
  }

  std::vector<Event> next_events;
  env_.event_queue().NextEvents(&next_events);

  // Push all of the events for the current time-step to their target
  // logical processes.
  std::set<int> active_processes;
  std::vector<Event>::iterator next_event(next_events.begin()),
      end(next_events.end());
  for (; next_event != end; ++next_event) {
    active_processes.insert(next_event->target_process_id());
    env_.process(next_event->target_process_id())->ReceiveEvent(*next_event);
  }

  // Evaluate all of the input-queues for each process which has work.
  std::set<int>::iterator active_proc(active_processes.begin()),
    active_end(active_processes.end());
  for (; active_proc != active_end; ++active_proc) {
    env_.process(*active_proc)->EvaluateInputQueue(&env_);
  }
}

Time SimulationEngine::LocalVirtualTime() const {
  // The virtual time is the virtual time of the process environment:
  // the timestamp of the smallest message in all of the input queues.
  Time min_time = env_.VirtualTime();
  Time message_ack_time;

  // Check to see if any marked sent messages which need to be accounted
  // exist.
  if (post_master_->LocalVirtualTimeContribution(&message_ack_time)) {
    min_time = (message_ack_time < min_time) ? message_ack_time : min_time;
  }
  return min_time;
}

void SimulationEngine::ReceiveGlobalVirtualTime(Time gvt) {
  assert(gvt <= LocalVirtualTime());
  env_.FossilCollect(gvt);
}

bool SimulationEngine::Finished() const {
  // TODO:  Implement somehow
  return false;
}