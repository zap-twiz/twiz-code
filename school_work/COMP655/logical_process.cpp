
#include "stdafx.h"

#include "logical_process.h"

#include "process_environment.h"

#include <algorithm>
#include <assert.h>
#include <iostream>

LogicalProcess::~LogicalProcess() {
  // Release all of the resources allocated by the states.
  std::vector<State*>::iterator iter(states_.begin()), end(states_.end());
  for (; iter != end; ++iter) {
    delete *iter;
  }
  states_.clear();
}

void LogicalProcess::ProcessEvent(Event const & event,
                                  ProcessEnvironment* process_environment) {
  // This is a future event - assume arbitrary/safe processing of
  // same-timestamp events
  if (event.receive_time_stamp() >= LogicalTime()) {
    if (event.is_positive()) {
      PushState(BuildMemento());
      locally_sent_events_.clear();
      local_time_ = Evaluate(event, process_environment);
      PostProcessEvent(event);
    } else {  // event.is_negative()
      // We've received an anti-message from the future!
      // Positive message is in-bound
      assert(false);
    }
  } else {
    std::vector<Event> coast_forward_events;
    Rollback(event.receive_time_stamp(), &coast_forward_events,
             process_environment);

    if (event.is_positive()) {
      // process a straggler
      ProcessEvent(event, process_environment);
    } else { // event.is_negative()
      // Remove the annihilated event from the coast-forward set
      Event positive_version = event;
      positive_version.negate();
      assert(positive_version.is_positive());

      std::vector<Event>::iterator found =
          std::find(coast_forward_events.begin(), coast_forward_events.end(),
                    positive_version);
      assert(coast_forward_events.end() != found);
      coast_forward_events.erase(found);
    }

    std::vector<Event>::iterator coast_iter(coast_forward_events.begin()),
        coast_end(coast_forward_events.end());
    for (; coast_iter != coast_end; ++coast_iter) {
      process_environment->Send(*coast_iter);
    }
  }
}

void LogicalProcess::PostProcessEvent(Event const& event) {
  processed_events_.push_back(event);
  sent_events_.push_back(locally_sent_events_);
  locally_sent_events_.clear();
}

void LogicalProcess::SendEvent(Event const& event,
                               ProcessEnvironment* process_environment) {
  // Negate this event in the output queue
  locally_sent_events_.push_back(event);
  locally_sent_events_.back().negate();

#if 0
  if (name() == "Input 1" ) {
  //if (event.target_process_id() != id()) {
    std::cout << name() << " sending@: " << event.receive_time_stamp() <<
        " to " << event.target_process_id() << 
        " data: " << event.payload() << " type: " << event.type() << std::endl;
  //}
  }
#endif
  process_environment->Send(event);
}

void LogicalProcess::Rollback(Time time,
                              std::vector<Event>* to_re_evaluate,
                              ProcessEnvironment* process_environment) {
  assert(states_.size() == processed_events_.size() &&
         sent_events_.size() == states_.size());

  State* target_state = NULL;
  while (!processed_events_.empty()) {
    Event& most_recent = processed_events_.back();
    if (most_recent.receive_time_stamp() >= time) {
      // This state wasn't necessary, and will be re-computed
      ResurrectMemento(states_.back());
      delete states_.back();
      states_.pop_back();

      // send all of the anti-messages here!
      std::vector<Event>::iterator
          anti_message_iter(sent_events_.back().begin()),
          anti_message_end(sent_events_.back().end());
      for (; anti_message_iter != anti_message_end; ++anti_message_iter) {
        process_environment->Send(*anti_message_iter);
      }
      sent_events_.pop_back();

      to_re_evaluate->push_back(most_recent);
      processed_events_.pop_back();
    } else {
      break;
    }
  }

  std::cout << name() << " rolled to " << LogicalTime() << std::endl;
}
