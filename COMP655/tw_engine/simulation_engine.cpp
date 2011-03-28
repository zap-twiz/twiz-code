

#include "stdafx.h"
#include "simulation_engine.h"

#include "logical_process.h"

#include <assert.h>
#include <set>

void SimulationEngine::TimeStep() {
  if (env_.IsIdle())
    return;

  std::vector<Event> next_events;
  env_.event_queue().NextEvents(&next_events);

  std::set<int> active_processes;

  std::vector<Event>::iterator next_event(next_events.begin()),
      end(next_events.end());
  for (; next_event != end; ++next_event) {
    active_processes.insert(next_event->target_process_id());
    env_.process(next_event->target_process_id())->ReceiveEvent(*next_event);
  }

  std::set<int>::iterator active_proc(active_processes.begin()),
    active_end(active_processes.end());
  for (; active_proc != active_end; ++active_proc) {
    env_.process(*active_proc)->EvaluateInputQueue(&env_);
  }
}

Time SimulationEngine::LocalVirtualTime() const {
  Time min_time = env_.VirtualTime();
  Time message_ack_time;
  if (post_master_->LocalVirtualTimeContribution(&message_ack_time)) {
    min_time = message_ack_time < min_time ? message_ack_time : min_time;
  }
  return min_time;
}

void SimulationEngine::ReceiveGlobalVirtualTime(Time gvt) {
  assert(gvt <= LocalVirtualTime());

  // Fossil Collect
  env_.FossilCollect(gvt);
}
