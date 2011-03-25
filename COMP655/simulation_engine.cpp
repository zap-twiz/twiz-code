

#include "stdafx.h"
#include "simulation_engine.h"

#include "event.h"
#include "logical_process.h"

#include <assert.h>

void SimulationEngine::TimeStep() {
  if (env_.Idle())
    return;

  std::vector<Event> next_pos_events, next_neg_events;
  env_.event_queue().NextEvents(&next_pos_events, &next_neg_events);

  // Process all positive events.
  std::vector<Event>::iterator pos_iter(next_pos_events.begin()),
      pos_end(next_pos_events.end());
  for (; pos_iter != pos_end; ++pos_iter) {
    env_.process(pos_iter->target_process_id())->ProcessEvent(*pos_iter, &env_);
  }

  std::vector<Event> future_negatives;
  std::vector<Event>::iterator neg_iter(next_neg_events.begin()),
      neg_end(next_neg_events.end());
  for (; neg_iter != neg_end; ++neg_iter) {
    LogicalProcess* process = env_.process(neg_iter->target_process_id());
    if (process->LogicalTime() > neg_iter->receive_time_stamp()) {
      process->ProcessEvent(*neg_iter, &env_);
    } else {
      future_negatives.push_back(*neg_iter);
    }
  }

  // Re-post any future negative events
  std::vector<Event>::iterator future_iter(future_negatives.begin()),
      future_end(future_negatives.end());
  for (; future_iter != future_end; ++future_iter) {
    env_.Send(*future_iter);
    assert(false);
  }
}
