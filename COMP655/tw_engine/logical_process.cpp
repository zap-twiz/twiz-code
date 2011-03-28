
#include "stdafx.h"

#include "logical_process.h"

#include "process_environment.h"

#include <algorithm>
#include <assert.h>
#include <iostream>

namespace {

void PositiveNegativeIntersect(std::vector<Event> const & pos_events,
                               std::vector<Event> const & neg_events,
                               std::vector<Event>* intersection ) {
  intersection->clear();

  std::vector<Event>::const_iterator neg_iter(neg_events.begin()),
    neg_end(neg_events.end());
  for (; neg_iter != neg_end; ++neg_iter) {
    Event positive_equivalent = *neg_iter;
    positive_equivalent.negate();

    std::vector<Event>::const_iterator found_pos = std::find(
        pos_events.begin(), pos_events.end(), positive_equivalent);
    if (found_pos != pos_events.end()) {
      intersection->push_back(positive_equivalent);
    }
  }
}

void DoAnnihilation(std::vector<Event> const & intersection,
                    std::vector<Event>* events) {
  std::vector<Event>::const_iterator annihil_iter(intersection.begin()),
    annihil_end(intersection.end());
  for (; annihil_iter != annihil_end; ++annihil_iter) {
    assert(annihil_iter->is_positive());
    // Erase both the positive and negative version of the event.
    std::vector<Event>::iterator found_pos =
        std::find(events->begin(), events->end(), *annihil_iter);
    assert(found_pos != events->end());
    events->erase(found_pos);

    Event negative_event = *annihil_iter;
    negative_event.negate();
    std::vector<Event>::iterator found_neg =
        std::find(events->begin(), events->end(), negative_event);
    assert(found_neg != events->end());
    events->erase(found_neg);
  }
}

void ResolveInputQueue(std::vector<Event>* input_queue) {
  // First pull apart the postive and negative events.
  std::vector<Event> positive_events, negative_events;
  std::vector<Event>::iterator input_iter(input_queue->begin()),
      input_end(input_queue->end());
  for (; input_iter != input_end; ++input_iter) {
    if (input_iter->is_positive()) {
      positive_events.push_back(*input_iter);
    } else {
      negative_events.push_back(*input_iter);
    }
  }

  // Compute the set of matching positive/negative event
  // pairs.
  std::vector<Event>  intersection;
  PositiveNegativeIntersect(positive_events, negative_events,
                            &intersection);

  // Remove all events corresponding to matching positive
  // negative pairs.
  DoAnnihilation(intersection, input_queue);
}

}  // namespace


LogicalProcess::~LogicalProcess() {
  // Release all of the resources allocated by the states.
  std::vector<State*>::iterator iter(states_.begin()), end(states_.end());
  for (; iter != end; ++iter) {
    delete *iter;
  }
  states_.clear();
}

void LogicalProcess::ReceiveEvent(Event const & event) {
  //assert(event.receive_time_stamp() == input_queue_.back().receive_time_stamp);
  input_queue_.push_back(event);
}

void LogicalProcess::EvaluateInputQueue(
    ProcessEnvironment* process_environment) {
  ResolveInputQueue(&input_queue_);

  std::vector<Event> events_postponed;

  std::vector<Event>::iterator input_iter(input_queue_.begin()),
    input_end(input_queue_.end());
  for (; input_iter != input_end; ++input_iter) {
    if (!ProcessEvent(*input_iter, process_environment)) {
      events_postponed.push_back(*input_iter);
    }
  }

  input_queue_ = events_postponed;
}

bool LogicalProcess::ProcessEvent(
    Event const & event,
    ProcessEnvironment* process_environment) {
  bool event_consumed = false;
  // This is a future event - assume arbitrary/safe processing of
  // same-timestamp events
  if (event.receive_time_stamp() >= LogicalTime()) {
    event_consumed = ProcessFutureEvent(event, process_environment);
  } else {  // event is a straggler anti-message, or message
    event_consumed = ProcessHistoricalEvent(event, process_environment);
  }

  return event_consumed;
}

bool LogicalProcess::ProcessFutureEvent(
    Event const & event,
    ProcessEnvironment* process_environment) {
  assert(event.receive_time_stamp() >= LogicalTime());
  bool event_consumed = false;
  if (event.is_positive()) {
    PushState(BuildMemento());
    locally_sent_events_.clear();
    local_time_ = Evaluate(event, process_environment);
    PostProcessEvent(event);
    event_consumed = true;
  } else {  // event.is_negative()
    // We've received an anti-message from the future!
    // Positive message is in-bound
    assert(false);
  }
  return event_consumed;
}

bool LogicalProcess::ProcessHistoricalEvent(
    Event const & event,
    ProcessEnvironment* process_environment) {
  // If the message is an anti-event, for which no corresponding event
  // has been processed, then we have a late/straggler message
  if (event.is_negative()) {
    Event positive_version = event;
    positive_version.negate();

    std::vector<Event>::iterator found_event =
      std::find(processed_events_.begin(), processed_events_.end(),
                positive_version);
    if (found_event == processed_events_.end())
      return false;
  }

  std::vector<Event> coast_forward_events;
  Rollback(event.receive_time_stamp(), &coast_forward_events,
            process_environment);

  if (event.is_positive()) {
    // Process a straggler event.
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

  // Re-post the coast-forward events.
  std::vector<Event>::iterator coast_iter(coast_forward_events.begin()),
      coast_end(coast_forward_events.end());
  for (; coast_iter != coast_end; ++coast_iter) {
    process_environment->Send(*coast_iter);
  }

  return true;
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
    if (most_recent.receive_time_stamp() > time) {
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

Time LogicalProcess::MinVirtualTime() const {
  Time min_time = LogicalTime();

  std::vector<Event>::const_iterator input_iter(input_queue_.begin()),
      input_end(input_queue_.end());
  for (; input_iter != input_end; ++input_iter) {
    if (input_iter->receive_time_stamp() < min_time) {
      min_time = input_iter->receive_time_stamp();
    }
  }

  return min_time;
}

void LogicalProcess::FossilCollect(Time gvt) {
  assert(states_.size() == processed_events_.size() &&
         sent_events_.size() == states_.size());

  // Clear all states, processed events, and sent events
  while (!processed_events_.empty() &&
         processed_events_.front().receive_time_stamp() < gvt) {
    processed_events_.erase(processed_events_.begin());
    sent_events_.erase(sent_events_.begin());

    delete states_.front();
    states_.erase(states_.begin());
  }

  // Make sure that the memory was actually reclaimed.
  processed_events_.shrink_to_fit();
  sent_events_.shrink_to_fit();
  states_.shrink_to_fit();
}
